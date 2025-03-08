import cv2
import psutil
import time
import glob
from threading import Thread, Lock, Event
from flask import Flask, Response

app = Flask(__name__)

# Global dictionaries to store camera frames, locks, threads, stop events, and usage counts.
camera_feeds = {}         # {camera_index: latest JPEG frame bytes}
camera_locks = {}         # {camera_index: Lock used when writing/reading the frame}
camera_threads = {}       # {camera_index: Thread object for capturing frames}
camera_stop_events = {}   # {camera_index: Event to signal the camera thread to stop}
camera_usage_counts = {}  # {camera_index: number of clients currently requesting this feed}
usage_lock = Lock()       # Global lock to protect changes to camera_usage_counts

# A set of valid camera indices that we detect at startup.
valid_cameras = set()

def list_valid_cameras():
    """
    Scans /dev for video devices (e.g. /dev/video0, /dev/video1, etc.)
    and checks their availability. Returns a list of camera indices.
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
    Monitors and prints the network bandwidth usage.
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

def camera_thread(camera_index, stop_event):
    """
    Opens the camera and continuously captures frames until the stop_event is set.
    Each captured frame is encoded as JPEG and stored in camera_feeds.
    """
    device = f"/dev/video{camera_index}"
    print(f"Starting thread for camera at {device}")
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
    cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc(*'MJPG'))
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
        if not success:
            print(f"Failed to read frame from camera at {device}")
            time.sleep(1)
            continue

        with camera_locks[camera_index]:
            ret, buffer = cv2.imencode('.jpg', frame, compression_params)
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
                frame = camera_feeds.get(camera_index, None)
            if frame:
                yield (b'--frame\r\n'
                       b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')
            time.sleep(0.067)  # Approximately 15 FPS
    finally:
        with usage_lock:
            camera_usage_counts[camera_index] -= 1
            print(f"Client disconnected from camera {camera_index}. Usage count: {camera_usage_counts[camera_index]}")
            if camera_usage_counts[camera_index] <= 0:
                # No more clients – signal the camera thread to stop.
                if camera_index in camera_stop_events:
                    camera_stop_events[camera_index].set()
                    print(f"Stopping camera {camera_index} as no clients are connected.")

@app.route('/video_feed/<int:camera_index>')
def video_feed(camera_index):
    """
    Flask endpoint for the video feed. It ensures the camera is valid,
    creates/starts the camera thread if needed, and increments the usage count.
    """
    if camera_index not in valid_cameras:
        return f"Camera {camera_index} not available.", 404

    # Ensure there is a lock for this camera.
    if camera_index not in camera_locks:
        camera_locks[camera_index] = Lock()

    with usage_lock:
        # Initialize or increment the usage count.
        if camera_index not in camera_usage_counts:
            camera_usage_counts[camera_index] = 0
        camera_usage_counts[camera_index] += 1
        print(f"Client connected to camera {camera_index}. Usage count: {camera_usage_counts[camera_index]}")

        # Start the camera thread if it isn’t running.
        if camera_index not in camera_threads or not camera_threads[camera_index].is_alive():
            stop_event = Event()
            camera_stop_events[camera_index] = stop_event
            camera_threads[camera_index] = Thread(target=camera_thread, args=(camera_index, stop_event), daemon=True)
            camera_threads[camera_index].start()

    return Response(generate_frames(camera_index),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    # Detect valid cameras at startup (but do not start their threads yet).
    valid = list_valid_cameras()
    if not valid:
        print("No valid cameras detected. Exiting.")
        exit(1)
    valid_cameras = set(valid)

    # Start the bandwidth monitor in a separate thread.
    Thread(target=monitor_bandwidth, daemon=True).start()

    # Run the Flask application.
    app.run(host='0.0.0.0', port=5000)
