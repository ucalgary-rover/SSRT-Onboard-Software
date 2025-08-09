import cv2
import psutil
import time
import glob
from threading import Thread, Lock, Event
from flask import Flask, Response, request

# — ArUco setup with fallbacks —
try:
    import cv2.aruco as aruco
except ImportError:
    raise RuntimeError(
        "cv2.aruco module not found! Please install with: pip install opencv-contrib-python"
    )

ARUCO_DICT = aruco.getPredefinedDictionary(aruco.DICT_6X6_250)

ARUCO_PARAMS = aruco.DetectorParameters()

app = Flask(__name__)

# Global dictionaries to store camera frames, locks, threads, stop events, usage counts, and detect flags.
camera_feeds = {}  # {camera_index: latest JPEG frame bytes}
camera_locks = {}  # {camera_index: Lock}
camera_threads = {}  # {camera_index: Thread}
camera_stop_events = {}  # {camera_index: Event}
camera_usage_counts = {}  # {camera_index: number of clients}
camera_detect_flags = {}  # {camera_index: bool}
usage_lock = Lock()  # Protects camera_usage_counts
valid_cameras = set()  # Populated at startup


def list_valid_cameras():
    """
    Scans /dev for video devices (e.g. /dev/video0, /dev/video1, etc.)
    and returns a list of indices for those that open successfully.
    """
    valid = []
    video_devices = glob.glob("/dev/video*")
    for device in video_devices:
        try:
            idx = int(device.replace("/dev/video", ""))
        except ValueError:
            continue  # Skip any device that doesn't follow the expected naming
        cap = cv2.VideoCapture(device, cv2.CAP_V4L2)
        if cap.isOpened():
            print(f"Camera at {device} is available.")
            valid.append(idx)
            cap.release()
        else:
            print(f"Camera at {device} is not available.")
    return valid


def monitor_bandwidth():
    """
    Monitors and prints the network bandwidth usage every second.
    """
    net_before = psutil.net_io_counters()
    bytes_sent_before = net_before.bytes_sent
    bytes_recv_before = net_before.bytes_recv

    while True:
        time.sleep(1)
        net_after = psutil.net_io_counters()
        bytes_sent_after = net_after.bytes_sent
        bytes_recv_after = net_after.bytes_recv

        sent = bytes_sent_after - bytes_sent_before
        received = bytes_recv_after - bytes_recv_before

        bytes_sent_before = bytes_sent_after
        bytes_recv_before = bytes_recv_after

        sent_kbps = (sent * 8) / 1024
        received_kbps = (received * 8) / 1024
        print(f"Sent: {sent_kbps:.2f} kbps, Received: {received_kbps:.2f} kbps")


def camera_thread(camera_index, stop_event, detect=False):
    """
    Captures frames from /dev/video<camera_index>, optionally performs ArUco detection,
    encodes to JPEG, and stores in camera_feeds until stop_event is set.
    """
    device = f"/dev/video{camera_index}"
    print(f"Starting thread for camera at {device} (detect={detect})")
    cap = cv2.VideoCapture(device, cv2.CAP_V4L2)

    # Retry logic to ensure the camera opens.
    for attempt in range(3):
        if cap.isOpened():
            break
        print(f"Retrying to open camera at {device} (Attempt {attempt + 1})")
        time.sleep(1)
        cap = cv2.VideoCapture(device, cv2.CAP_V4L2)

    if not cap.isOpened():
        print(f"Failed to open camera at {device} after retries.")
        return

    # Set codec and properties.
    cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*"MJPG"))
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
    cap.set(cv2.CAP_PROP_FPS, 15)

    # # Warm-up: capture and discard a few frames. -- USE FOR TESTING
    # for i in range(10):
    #     success, _ = cap.read()
    #     if success:
    #         print(f"Camera at {device} warm-up frame {i} captured.")
    #     else:
    #         print(f"Camera at {device} warm-up frame {i} failed.")
    #     time.sleep(0.1)

    compression_params = [cv2.IMWRITE_JPEG_QUALITY, 50]

    while not stop_event.is_set():
        success, frame = cap.read()
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        if not success:
            print(f"Failed to read frame from camera at {device}")
            time.sleep(1)
            continue

        # ArUco detection & drawing
        if detect:
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            corners, ids, _ = aruco.detectMarkers(
                gray, ARUCO_DICT, parameters=ARUCO_PARAMS
            )
            if ids is not None:
                aruco.drawDetectedMarkers(frame, corners, ids)

        # Convert to RGB before encoding
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        with camera_locks[camera_index]:
            ret, buffer = cv2.imencode(".jpg", frame, compression_params)
            if ret:
                camera_feeds[camera_index] = buffer.tobytes()
            else:
                print(f"Failed to encode frame from camera at {device}")
    cap.release()
    print(f"Camera at {device} thread exiting.")


def generate_frames(camera_index):
    """
    Generator function that yields JPEG frames for the specified camera.
    When the client disconnects, it decrements the usage count and, if no clients remain,
    signals the camera thread to stop.
    """
    try:
        while True:
            with camera_locks[camera_index]:
                frame = camera_feeds.get(camera_index)
            if frame:
                yield (
                    b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"
                )
            time.sleep(0.067)  # Approximately 15 FPS
    finally:
        with usage_lock:
            camera_usage_counts[camera_index] -= 1
            print(
                f"Client disconnected from camera {camera_index}. Usage count: {camera_usage_counts[camera_index]}"
            )
            if camera_usage_counts[camera_index] <= 0:
                # No more clients – signal the camera thread to stop.
                if camera_index in camera_stop_events:
                    camera_stop_events[camera_index].set()
                    print(
                        f"Stopping camera {camera_index} as no clients are connected."
                    )


@app.route("/video_feed/<int:camera_index>")
def video_feed(camera_index):
    """
    HTTP endpoint: stream the MJPEG feed for camera_index.
    Accepts an optional ?detect=1 to enable ArUco marker drawing.
    """
    if camera_index not in valid_cameras:
        return f"Camera {camera_index} not available.", 404

    # parse detect flag (default off)
    detect = request.args.get("detect", "0") == "1"
    camera_locks.setdefault(camera_index, Lock())

    with usage_lock:
        camera_usage_counts[camera_index] = camera_usage_counts.get(camera_index, 0) + 1
        prev_thread = camera_threads.get(camera_index)
        prev_detect = camera_detect_flags.get(camera_index, False)

        # (Re)start thread if needed or if detect-mode changed
        if not prev_thread or not prev_thread.is_alive() or (detect != prev_detect):
            if prev_thread and prev_thread.is_alive():
                camera_stop_events[camera_index].set()

            stop_event = Event()
            camera_stop_events[camera_index] = stop_event
            camera_detect_flags[camera_index] = detect
            t = Thread(
                target=camera_thread,
                args=(camera_index, stop_event, detect),
                daemon=True,
            )
            camera_threads[camera_index] = t
            t.start()

    return Response(
        generate_frames(camera_index),
        mimetype="multipart/x-mixed-replace; boundary=frame",
    )


if __name__ == "__main__":
    # Detect valid cameras at startup
    cams = list_valid_cameras()
    if not cams:
        print("No valid cameras detected. Exiting.")
        exit(1)
    valid_cameras = set(cams)
    print("Found cameras:", sorted(valid_cameras))

    # Start the bandwidth monitor in a separate thread.
    Thread(target=monitor_bandwidth, daemon=True).start()

    # Run the Flask application.
    app.run(host="0.0.0.0", port=5000)
