# Running the Camera server

### Note this README assumes you are using Ubuntu, as to mimic running this on the Rover's NUC

#### To check which Python packages are currently installed on your machine run the following command:

```
pip list
```

1. Run install.sh

```bash
./install.sh
```
if necessary run:
```bash
chmod +x install.sh
```

2. Run the server using the following command (once you're in the directory the file is in, in the terminal):

```
python3 CameraFeedServer.py
```
