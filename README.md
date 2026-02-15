# SSRT-Onboard-Software
Repository for storing code that will be run on the rover.

## Setup

Make sure you have an SSH key for your GitHub account. If you don't, see the [Telemetry Instructions](https://github.com/ucalgary-rover/SSRT-Telemetry/blob/main/docs/project-setup.md) for instructions on how to set one up.

Clone the repository using

```bash
mkdir Your/Desired/Directory
cd Your/Desired/Directory/
git clone --recurse-submodules git@github.com/ucalgary-rover/SSRT-Onboard-Software
cd SSRT-Onboard-Software
```

Or if already cloned, and needing to update submodules:
```bash
git submodule update --init --recursive
```

Install required libraries for precommit
```bash
sudo apt install clang-format clang-tidy
```

## CameraFeed
Code for the camera feed servers. Uses a Python Flask server to stream data from all video ports available

### Install
Navigate to the `CameraFeed` directory. Run the following
```
./insall.sh
```

Wait for the install to complete, then follow the instructions from the install script.

## MQTTServer
Code to read from the sensors and publish that data to an MQTT server. All commands below will be run in the `MQTTServer` directory.

### Install Paho-MQTT
1. Make sure `install-paho.sh` is executable
```bash
chmod +x install-paho.sh
```

2. Run `install-paho.sh`
```bash
./install-paho.sh
```

### Build

1. The first time you build, create the build directory using
```bash
mkdir build
```

2. Enter the build directory
```bash
cd build
```

3. Create the build files
```bash
cmake ..
```

4. Build the executable
```bash
make
```

5. Run the code
```bash
cd ..
./mqtt-server
```

### Contributing
- Please make sure that all files are included in the correct folder.
- If you have extra libraries that are required, please make sure they are either (a) included in this repo and/or (b) instructions are added to this README for installation
- Ensure the code to read from the sensor and the code to publish to the MQTT server remain separated

## Sensors
Arduino code for reading from sensors. Structured so each sensor is read from, then data is sent to the NUC in one push. Data for the NUC should be sent over serial in a JSON-like format
