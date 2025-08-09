#!/bin/bash
set -e  # Exit on error

# Variables
PAHO_MQTT_C_REPO="https://github.com/eclipse/paho.mqtt.c.git"
PAHO_MQTT_CPP_REPO="https://github.com/eclipse/paho.mqtt.cpp.git"
C_DIR="paho.mqtt.c"
CPP_DIR="paho.mqtt.cpp"
BUILD_DIR="build"
INSTALL_PREFIX="/usr/local"

# Function to build and install MQTT C library
install_mqtt_c() {
  echo "=== Installing Paho MQTT C library ==="
  if [ ! -d "$C_DIR" ]; then
    git clone "$PAHO_MQTT_C_REPO"
  else
    echo "$C_DIR already exists, skipping clone."
  fi

  cd "$C_DIR"
  mkdir -p "$BUILD_DIR"
  cd "$BUILD_DIR"

  echo "Configuring MQTT C build..."
  cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" -DPAHO_WITH_SSL=ON -DCMAKE_C_STANDARD=99 -DCMAKE_C_STANDARD_REQUIRED=ON -DCMAKE_C_FLAGS="-std=c99" ..
  echo "Building MQTT C library..."
  cmake --build . -- -j$(nproc)

  echo "Installing MQTT C library..."
  sudo cmake --install .

  cd ../..  # Return to root
}

# Function to build and install MQTT C++ library
install_mqtt_cpp() {
  echo "=== Installing Paho MQTT C++ library ==="
  if [ ! -d "$CPP_DIR" ]; then
    git clone "$PAHO_MQTT_CPP_REPO"
  else
    echo "$CPP_DIR already exists, skipping clone."
  fi

  cd "$CPP_DIR"
  mkdir -p "$BUILD_DIR"
  cd "$BUILD_DIR"

  echo "Configuring MQTT C++ build..."
  cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" -DCMAKE_C_STANDARD=99 -DCMAKE_C_STANDARD_REQUIRED=ON -DCMAKE_C_FLAGS="-std=c99" ..
  echo "Building MQTT C++ library..."
  cmake --build . -- -j$(nproc)

  echo "Installing MQTT C++ library..."
  sudo cmake --install .

  cd ../..  # Return to root
}

# Run both installs
install_mqtt_c
install_mqtt_cpp

echo "=== Installation complete! ==="
