# SSRT-Onboard-Software
Repository for storing code that will be run on the rover.

## CameraFeed
Code for the camera feed servers. Uses a Python Flask server to stream data from all video ports available

### Install
Navigate to the `CameraFeed` directory. Run the following
```
./insall.sh
```

Wait for the install to complete, then follow the instructions from the install script.

## MQTTServer
Code to read from the sensors and publish that data to an MQTT server

## Sensors
Arduino code for reading from sensors. Structured so each sensor is read from, then data is sent to the NUC in one push. Data for the NUC should be sent over serial in a JSON-like format


