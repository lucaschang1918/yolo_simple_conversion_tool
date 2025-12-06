import sys
import os
import cv2
import torch
import numpy as np

CAMID = 1
QUIT_FILE = "quit.flag"

def should_quit():
    return os.path.exists(QUIT_FILE)

def load_yolov5_native(weights_path):
    """使用纯PyTorch方式加载YOLOv5模型,不依赖ultralytics"""
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    
    # 加载模型权重
    ckpt = torch.load(weights_path, map_location=device)
    
    # 获取模型
    if isinstance(ckpt, dict):
        model = ckpt.get('ema') or ckpt.get('model')
        model = model.float()
    else:
        model = ckpt.float()
    
    model.to(device).eval()
    
    # 获取类别名称
    names = model.names if hasattr(model, 'names') else [f'class{i}' for i in range(1000)]
    
    return model, device, names

def letterbox(img, new_shape=(640, 640), color=(114, 114, 114)):
    """调整图像大小并填充"""
    shape = img.shape[:2]
    if isinstance(new_shape, int):
        new_shape = (new_shape, new_shape)
    
    r = min(new_shape[0] / shape[0], new_shape[1] / shape[1])
    new_unpad = int(round(shape[1] * r)), int(round(shape[0] * r))
    dw, dh = new_shape[1] - new_unpad[0], new_shape[0] - new_unpad[1]
    dw /= 2
    dh /= 2
    
    if shape[::-1] != new_unpad:
        img = cv2.resize(img, new_unpad, interpolation=cv2.INTER_LINEAR)
    
    top, bottom = int(round(dh - 0.1)), int(round(dh + 0.1))
    left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    img = cv2.copyMakeBorder(img, top, bottom, left, right, cv2.BORDER_CONSTANT, value=color)
    
    return img, r, (dw, dh)

def non_max_suppression(prediction, conf_thres=0.25, iou_thres=0.45, max_det=300):
    """NMS后处理"""
    bs = prediction.shape[0]
    nc = prediction.shape[2] - 5
    xc = prediction[..., 4] > conf_thres
    
    output = [torch.zeros((0, 6), device=prediction.device)] * bs
    
    for xi, x in enumerate(prediction):
        x = x[xc[xi]]
        if not x.shape[0]:
            continue
        
        x[:, 5:] *= x[:, 4:5]
        box = x[:, :4]
        conf, j = x[:, 5:].max(1, keepdim=True)
        x = torch.cat((box, conf, j.float()), 1)[conf.view(-1) > conf_thres]
        
        n = x.shape[0]
        if not n:
            continue
        
        c = x[:, 5:6] * 7680
        boxes, scores = x[:, :4] + c, x[:, 4]
        i = torch.ops.torchvision.nms(boxes, scores, iou_thres)
        if i.shape[0] > max_det:
            i = i[:max_det]
        
        output[xi] = x[i]
    
    return output

def draw_boxes(img, detections, names):
    """在图像上绘制检测框"""
    for det in detections:
        if len(det):
            for *xyxy, conf, cls in det:
                label = f'{names[int(cls)]} {conf:.2f}'
                x1, y1, x2, y2 = map(int, xyxy)
                cv2.rectangle(img, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(img, label, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 
                           0.5, (0, 255, 0), 2)
    return img

def main():
    if len(sys.argv) < 3:
        print("Usage: python test_pt.py <weights> <version>")
        return

    weights = sys.argv[1]
    version = sys.argv[2]
    print(f"Loading model: {weights} (version: {version})")

    ver = version.lower()
    
    # --- YOLOv5 使用原生PyTorch加载 ---
    if "yolov5" in ver:
        try:
            print("Loading YOLOv5 model with native PyTorch...")
            model, device, names = load_yolov5_native(weights)
            print(f"✅ Model loaded on {device}")
            use_native = True
        except Exception as e:
            print(f"❌ Failed to load model: {e}")
            import traceback
            traceback.print_exc()
            return
    
    # --- YOLOv8 / YOLOv11 使用 ultralytics ---
    elif "yolov8" in ver or "yolov11" in ver:
        try:
            from ultralytics import YOLO
            model = YOLO(weights)
            use_native = False
            print("✅ Model loaded with ultralytics")
        except Exception as e:
            print(f"❌ Failed to load model: {e}")
            return
    else:
        print("❌ Unknown YOLO version!")
        return

    # --- 打开摄像头 ---
    cap = cv2.VideoCapture(CAMID)
    if not cap.isOpened():
        print("❌ Unable to open camera")
        return

    print("📹 Camera opened. Press Q on window or trigger quit.flag to exit.")
    
    img_size = 640

    while True:
        if should_quit():
            print(">>> Received quit flag. Exiting...")
            break

        ret, frame = cap.read()
        if not ret:
            print("⚠️ Failed to read frame")
            continue

        try:
            if use_native and "yolov5" in ver:
                # YOLOv5 原生推理
                img0 = frame.copy()
                img, ratio, pad = letterbox(frame, img_size)
                img = img.transpose((2, 0, 1))[::-1]
                img = np.ascontiguousarray(img)
                img = torch.from_numpy(img).to(device)
                img = img.float() / 255.0
                if len(img.shape) == 3:
                    img = img[None]
                
                with torch.no_grad():
                    pred = model(img)[0]
                
                pred = non_max_suppression(pred, conf_thres=0.25, iou_thres=0.45)
                
                # 缩放坐标回原图
                for det in pred:
                    if len(det):
                        det[:, :4] -= torch.tensor([pad[0], pad[1], pad[0], pad[1]], device=device)
                        det[:, :4] /= ratio
                
                annotated_frame = draw_boxes(img0, pred, names)
            else:
                # YOLOv8/v11 使用 ultralytics
                results = model(frame, verbose=False)
                annotated_frame = results[0].plot()

            cv2.imshow("YOLO Test - Press Q to Quit", annotated_frame)
        except Exception as e:
            print(f"⚠️ Inference error: {e}")
            cv2.imshow("YOLO Test - Press Q to Quit", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

    if should_quit() and os.path.exists(QUIT_FILE):
        os.remove(QUIT_FILE)
        print("✅ Cleanup complete")

if __name__ == "__main__":
    main()