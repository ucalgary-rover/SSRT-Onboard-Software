# Running the Camera server

### Note this README assumes you are using Ubuntu, as to mimic running this on the Rover's NUC

#### To check which Python packages are currently installed on your machine run the following command:

```
pip list
```

1. Run install.sh to setup the virtual environment

```bash
./install.sh
```

if necessary run:

```bash
chmod +x install.sh
```

2. Activate the virtual environment using

```bash
source venv/bin/activate
```

3. Run the server using the following command (once you're in the directory the file is in, in the terminal):

```bash
python CameraFeedServer.py
```

4. Deactivate the virtual environment using

```bash
deactivate
```
