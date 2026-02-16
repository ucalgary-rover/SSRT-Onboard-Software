import cv2
import glob
from threading import Thread, Lock, Event
from flask import Flask, Response, request

try:
    import cv2.aruco as aruco
except ImportError:
    raise RuntimeError(
        "cv2.aruco module not found! Install with: pip install opencv-contrib-python"
    )

ARUCO_DICT = aruco.getPredefinedDictionary(aruco.DICT_6X6_250)
ARUCO_PARAMS = aruco.DetectorParameters()

app = Flask(__name__)

camera_feeds = {}
camera_locks = {}
camera_threads = {}
camera_stop_events = {}
camera_detect_flags = {}
valid_cameras = set()


def list_valid_cameras():
    valid = []
    for device in glob.glob("/dev/video*"):
        try:
            idx = int(device.replace("/dev/video", ""))
        except ValueError:
            continue
        cap = cv2.VideoCapture(device, cv2.CAP_V4L2)
        if cap.isOpened():
            valid.append(idx)
            cap.release()
    return valid


def camera_thread(camera_index, stop_event, detect=False):
    device = f"/dev/video{camera_index}"
    cap = cv2.VideoCapture(device, cv2.CAP_V4L2)
    cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    cap.set(cv2.CAP_PROP_FPS, 15)
    compression_params = [cv2.IMWRITE_JPEG_QUALITY, 50]

    while not stop_event.is_set():
        success, frame = cap.read()
        if not success:
            continue
        if detect:
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            corners, ids, _ = aruco.detectMarkers(
                gray, ARUCO_DICT, parameters=ARUCO_PARAMS
            )
            if ids is not None:
                aruco.drawDetectedMarkers(frame, corners, ids)
        ret, buffer = cv2.imencode(".jpg", frame, compression_params)
        if ret:
            with camera_locks[camera_index]:
                camera_feeds[camera_index] = buffer.tobytes()
    cap.release()


def generate_frames(camera_index):
    while True:
        with camera_locks[camera_index]:
            frame = camera_feeds.get(camera_index)
        if frame:
            yield b"--frame\r\nContent-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"


@app.route("/video_feed/<int:camera_index>")
def video_feed(camera_index):
    if camera_index not in valid_cameras:
        return f"Camera {camera_index} not available.", 404
    detect = request.args.get("detect", "0") == "1"
    camera_locks.setdefault(camera_index, Lock())
    prev_thread = camera_threads.get(camera_index)
    prev_detect = camera_detect_flags.get(camera_index, False)
    if not prev_thread or not prev_thread.is_alive() or (detect != prev_detect):
        if prev_thread and prev_thread.is_alive():
            camera_stop_events[camera_index].set()
        stop_event = Event()
        camera_stop_events[camera_index] = stop_event
        camera_detect_flags[camera_index] = detect
        t = Thread(
            target=camera_thread, args=(camera_index, stop_event, detect), daemon=True
        )
        camera_threads[camera_index] = t
        t.start()
    return Response(
        generate_frames(camera_index),
        mimetype="multipart/x-mixed-replace; boundary=frame",
    )


if __name__ == "__main__":
    cams = list_valid_cameras()
    if not cams:
        print("No valid cameras detected. Exiting.")
        exit(1)
    valid_cameras = set(cams)
    print("Found cameras:", sorted(valid_cameras))
    app.run(host="0.0.0.0", port=5000)
