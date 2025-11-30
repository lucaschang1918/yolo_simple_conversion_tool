# test_pt.py
import sys
import cv2
import os
import time
from ultralytics import YOLO

QUIT_FILE = "quit.flag"  # Qt 创建此文件时退出

def should_quit():
    return os.path.exists(QUIT_FILE)

def main():
    if len(sys.argv) < 2:
        print("Usage: python test_pt.py <weights>")
        return

    weights = sys.argv[1]
    print(f"Loading model: {weights}")
    model = YOLO(weights)

    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("❌ Unable to open camera")
        return

    print("Press Q on window or trigger Qt stop to exit.")

    while True:
        if should_quit():
            print(">>> Received quit flag. Exiting...")
            break

        ret, frame = cap.read()
        if not ret:
            continue

        results = model(frame)
        shown = results[0].plot()

        cv2.imshow("YOLO Test - Press Q to Quit", shown)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

    if should_quit():
        os.remove(QUIT_FILE)

if __name__ == "__main__":
    main()
