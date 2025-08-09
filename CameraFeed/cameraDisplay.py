# You may need to install requests: pip install requests
import cv2
import requests
import numpy as np

SERVER_URL = "http://192.168.1.100:5000"
MAX_CAMERAS = 8

def mjpeg_stream(url):
    stream = requests.get(url, stream=True)
    bytes_data = b""
    for chunk in stream.iter_content(chunk_size=1024):
        bytes_data += chunk
        a = bytes_data.find(b'\xff\xd8')
        b = bytes_data.find(b'\xff\xd9')
        if a != -1 and b != -1:
            jpg = bytes_data[a:b+2]
            bytes_data = bytes_data[b+2:]
            frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)
            yield frame

def main():
    for cam_idx in range(MAX_CAMERAS):
        url = f"{SERVER_URL}/video_feed/{cam_idx}"
        try:
            # Try to get one frame to check if the feed exists
            stream = mjpeg_stream(url)
            frame = next(stream)
            cv2.imshow(f"Camera {cam_idx}", frame)
        except Exception:
            continue

    while True:
        for cam_idx in range(MAX_CAMERAS):
            url = f"{SERVER_URL}/video_feed/{cam_idx}"
            try:
                stream = mjpeg_stream(url)
                frame = next(stream)
                cv2.imshow(f"Camera {cam_idx}", frame)
            except Exception:
                continue
        if cv2.waitKey(1) == 27:  # ESC to quit
            break
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()