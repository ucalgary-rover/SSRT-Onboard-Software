# Sensor development setup for MQTT data pipeline

General outline for building sensor interfaces that work with MQTT and Qt front end. This generic build uses an example found in paho.mqtt.cpp.

## Before beginning development
Run setup script (make sure git is installed):
```bash
./setup_mqtt.sh
```
In ArchLinux:
```bash
sudo chmod +x setup_mqtt.sh
./setup_mqtt.sh
```
This installs paho.mqtt.c and paho.mqtt.cpp sources in usr/local (relies on paho.mqtt.c)
