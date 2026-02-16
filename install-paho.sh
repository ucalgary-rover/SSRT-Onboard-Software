#!/usr/bin/env bash

# ensure apt is available
if ! command -v apt &> /dev/null; then
  echo "Error: this script assumes a Debian/Ubuntu system with apt."
  exit 1
fi

# install prerequisites
sudo apt-get update
sudo apt install git cmake build-essential libssl-dev mosquitto mosquitto-clients

# create install folder
INSTALL_DIR="paho-install"

mkdir $INSTALL_DIR
cd $INSTALL_DIR

# build Paho MQTT C from source
git clone https://github.com/eclipse/paho.mqtt.c
cd paho.mqtt.c
cmake -Bbuild -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=FALSE .
cmake --build build
sudo cmake --install build
cd ..

# build Paho MQTT C++ from source
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp
cmake -Bbuild -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=FALSE -DPAHO_MQTT_C_PATH=/usr/local .
cmake --build build
sudo cmake --install build
cd ..

# update system library paths
echo "/usr/local/lib" | sudo tee /etc/ld.so.conf.d/paho.conf
sudo ldconfig

# Cleanup
cd ..
rm -rf paho-install
