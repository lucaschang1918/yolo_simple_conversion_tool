# test_onnx.py
import sys
import cv2
import numpy as np
import os

QUIT_FILE = "quit.flag"

def should_quit():
    return os.path.exists(QUIT_FILE)

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_onnx.py <onnx_path>")
        return

    onnx_path = sys.argv[1]
    print(f"Loading ONNX: {onnx_path}")

    net = cv2.dnn.readNetFromONNX(onnx_path)

    cap = cv2.VideoCapture(0)
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

        blob = cv2.dnn.blobFromImage(frame, 1/255.0, (640, 640), swapRB=True)
        net.setInput(blob)
        out = net.forward()

        for det in out[0]:
            conf = det[4]
            if conf < 0.5:
                continue
            cx, cy = int(det[0]), int(det[1])
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
