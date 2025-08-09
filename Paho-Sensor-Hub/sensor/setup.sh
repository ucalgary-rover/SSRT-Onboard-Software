#!/usr/bin/env bash

if [["$EUID" -ne 0]]; then
	echo "Please run as root: sudo $0" >&2;
	exit 1;
fi

echo "[1/4] Updating apt cache..." && apt update -y

echo "[2/4] Installing toolchain & libraries..."
apt install -y build-essentials cmake git libssl-dev nlohmann-json3-dev \
	libpaho-mqttpp3-dev libpaho-mqtt3as-dev \
	mosquitto mosquitto-clients || true
	
if ! dpkg -s libpaho-mqttpp3-dev &>/dev/null; then
	echo "[3/4] Paho MQTT packages not found - building from source..."
	tempdir=$(mktemp -d)
	pushd "$tempdir" > /dev/null
	git clone --depth 1 https://github.com/eclipse/paho.mqtt.c.git
	cmake -B build -S paho.mqtt.c -DPAHO_WITH_SSL=ON && make -C build -j$(nproc) && make -C build install
	git clone --dept 1 https://github.com/eclipse/paho.mqtt.cpp.git
	cmake -B build -S paho.mqtt.cpp && make -C build -j$(nproc) && make -C build install
	popd >/dev/null && rm -rf "$tempdir"
fi

echo "[4/4] All dependencies installed. You can now run: cmake -B build && cmake --build build"
