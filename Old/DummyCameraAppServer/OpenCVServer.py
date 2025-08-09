import cv2
import psutil
import time
from threading import Thread  
from flask import Flask, Response

app = Flask(__name__)

def monitor_bandwidth():
    # Initial stats
    net_before = psutil.net_io_counters()
    bytes_sent_before = net_before.bytes_sent
    bytes_recv_before = net_before.bytes_recv

    while True:
        # Wait for a short interval
        time.sleep(1)  # Adjust this interval as needed

        # Capture stats after the interval
        net_after = psutil.net_io_counters()
        bytes_sent_after = net_after.bytes_sent
        bytes_recv_after = net_after.bytes_recv

        # Calculate bandwidth usage
        sent = bytes_sent_after - bytes_sent_before
        received = bytes_recv_after - bytes_recv_before

        # Update the baseline for the next interval
        bytes_sent_before = bytes_sent_after
        bytes_recv_before = bytes_recv_after

        # Convert to kilobits per second (kbps) for readability
        sent_kbps = (sent * 8) / 1024  # kbps
        received_kbps = (received * 8) / 1024  # kbps

        print(f"Sent: {sent_kbps:.2f} kbps, Received: {received_kbps:.2f} kbps")

def generate_frames(camera_num):
    cap = cv2.VideoCapture(camera_num)
    # cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
    # cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
    cap.set(cv2.CAP_PROP_FPS, 15)
    while True:
        success, frame = cap.read()
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame, [int(cv2.IMWRITE_JPEG_QUALITY), 50])
            frame = buffer.tobytes()
            # Yield the frame in byte format as part of an HTTP multipart response
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/video_feed/<int:camera_num>')
@app.route('/video_feed', defaults={'camera_num': 0})
def video_feed(camera_num):
    return Response(generate_frames(camera_num),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    Thread(target=monitor_bandwidth).start()
    app.run(host='0.0.0.0', port=5000)
