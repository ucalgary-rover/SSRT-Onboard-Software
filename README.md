# SSRT-Onboard-Software
Repository for storing code that will be run on the rover.

## Setup

Make sure you have an SSH key for your GitHub account. If you don't, see the [Telemetry Instructions](https://github.com/ucalgary-rover/SSRT-Telemetry/blob/main/docs/project-setup.md) for instructions on how to set one up.

Clone the repository using

```bash
mkdir Your/Desired/Directory
cd Your/Desired/Directory/
git clone git@github.com/ucalgary-rover/SSRT-Onboard-Software
cd SSRT-Onboard-Software
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

### Install
1. Install prerequisites

```bash
sudo apt-get update
sudo apt install git cmake build-essential libssl-dev mosquitto mosquitto-clients
```

2. Build Paho MQTT C from source
```bash
git clone https://github.com/eclipse/paho.mqtt.c
cd paho.mqtt.c
cmake -Bbuild -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=FALSE .
cmake --build build
sudo cmake --install build
cd ..
```

3. Build Paho MQTT C++ from source
```bash
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=FALSE -DPAHO_MQTT_C_PATH=/usr/local .
cmake --build build
sudo cmake --install build
cd ..
```

4. Update the system library paths
```bash
echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/paho.conf
sudo ldconfig
```

### Build
Run `make` to compile the code. Run `./publisher` to run the server.

### Contributing
- Please make sure that all files are included in the correct folder.
- If you have extra libraries that are required, please make sure they are either (a) included in this repo and/or (b) instructions are added to this README for installation
- Ensure the code to read from the sensor and the code to publish to the MQTT server remain separated

## Sensors
Arduino code for reading from sensors. Structured so each sensor is read from, then data is sent to the NUC in one push. Data for the NUC should be sent over serial in a JSON-like format
