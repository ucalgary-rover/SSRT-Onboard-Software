# Running the Camera server

### Note this README assumes you are using Ubuntu, as to mimic running this on the Rover's NUC

#### To check which Python packages are currently installed on your machine run the following command:

```
pip list
```

1. Make sure you have Python installed, if you don't have it, use the following command:

```
sudo apt update
sudo apt install python3
```

2. Make sure you have OpenCV installed, if you don't, use the following command:

```
sudo apt update
sudo apt install python3-opencv
```

3. Make sure you have Flask installed, if you don't, use the following command:

```
sudo apt update
sudo apt install python3-flask
```

4. Make sure you have psutil installed, if you don't, use the following command:

```
sudo apt update
sudo apt install python3-psutil
```

5. Run the server using the following command (once you're in the directory the file is in, in the terminal):

```
python3 CameraFeedServer.py
```
