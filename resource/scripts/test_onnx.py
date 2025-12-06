# test_onnx.py
import sys
import cv2
import numpy as np
import os
import onnxruntime as ort

CAMID = 1

QUIT_FILE = "quit.flag"

def should_quit():
    return os.path.exists(QUIT_FILE)

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_onnx.py <onnx_path>")
        return

    onnx_path = sys.argv[1]
    print(f"Loading ONNX: {onnx_path}")

    session = ort.InferenceSession(onnx_path, providers=['CPUExecutionProvider'])

    input_name = session.get_inputs()[0].name
    input_shape = session.get_inputs()[0].shape  # [1,3,320,320]
    h, w = input_shape[2], input_shape[3]

    cap = cv2.VideoCapture(CAMID)
    if not cap.isOpened():
        print("❌ Unable to open camera")
        return

    while True:
        if should_quit():
            print(">>> Received quit flag. Exiting...")
            break

        ret, frame = cap.read()
        if not ret:
            continue

        # === 🔥 Resize to model size (320×320) ===
        img = cv2.resize(frame, (w, h))
        blob = img.astype(np.float32) / 255.0
        blob = blob.transpose(2, 0, 1)  # CHW
        blob = np.expand_dims(blob, axis=0)

        outputs = session.run(None, {input_name: blob})
        preds = outputs[0][0]  # (N, 6) YOLO output

        for det in preds:
            conf = det[4]
            if conf < 0.5:
                continue

            cx, cy = int(det[0]), int(det[1])
            cx = int(cx / w * frame.shape[1])
            cy = int(cy / h * frame.shape[0])

            cv2.rectangle(frame, (cx-30, cy-30), (cx+30, cy+30), (0,255,0), 2)

        cv2.imshow("ONNX Test - Press Q to Quit", frame)
        if cv2.waitKey(1) & 0xff == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

    if should_quit():
        os.remove(QUIT_FILE)

if __name__ == "__main__":
    main()
