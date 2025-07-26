# MQTT Sensor Demo – C++ (Paho‑C++)

A minimal example that shows how to publish sensor data from C++ to a local **Mosquitto** broker using the **Eclipse Paho‑C++** client library.
Sensors send JSON payloads on `sensors/<sensorId>`; a separate *processor* client can subscribe, transform, and republish.

---

## 1. Quick start (Ubuntu 20.04 + newer)

```bash
# run the one‑liner to pull every dependency & broker
./setup.sh

# build & run
cmake -B build
cmake --build build -j$(nproc)

# terminal 1 – start the processor
./build/processor

# terminal 2 – start the first sensor
a./build/sensor   # change CLIENTID/TOPIC in the source to add more sensors

# terminal 3 – watch traffic
mosquitto_sub -t "#" -v
```

> **Tip:** You can keep Mosquitto running in a Docker container instead:
> `docker run -d --name mosquitto -p 1883:1883 eclipse-mosquitto`

---

## 2. Dependencies installed by `setup.sh`

| Package               | Why we need it                                     |
| --------------------- | -------------------------------------------------- |
| `build-essential`     | `g++`, `make`, standard toolchain                  |
| `cmake`               | Cross‑platform build system                        |
| `git`                 | To fetch source code                               |
| `libssl-dev`          | TLS support for Paho                               |
| `libpaho-mqtt3as-dev` | Paho MQTT‑C shared‑TLS client                      |
| `libpaho-mqttpp3-dev` | Paho MQTT‑C++ wrapper                              |
| `nlohmann-json3-dev`  | Lightweight JSON serialization                     |
| `mosquitto`           | Local MQTT broker                                  |
| `mosquitto-clients`   | CLI debug tools (`mosquitto_pub`, `mosquitto_sub`) |

If any of the `libpaho-*` packages are missing on your distro, the script automatically falls back to building them from source (takes \~2 min on a modern PC).

---

## 3. Manual install (if you don’t use the script)

```bash
sudo apt update && sudo apt install -y \
  build-essential cmake git libssl-dev nlohmann-json3-dev \
  libpaho-mqttpp3-dev libpaho-mqtt3as-dev \
  mosquitto mosquitto-clients
```

**macOS (Homebrew)**

```bash
brew install cmake paho-mqttpp nlohmann-json mosquitto
```

**Windows (vcpkg + MSVC)**

```powershell
vcpkg install paho-mqttpp:x64-windows nlohmann-json mosquitto
```

Then open the project in VS Code or Visual Studio and configure CMake with the vcpkg toolchain file.

---

## 4. Building & running

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Executables land in `build/`.
Launch `mosquitto -v` (or your Docker container), then run the sensor and processor binaries as shown in *Quick start*.

---

## 5. Customising

* **Sensor interval** – change `INTERVAL` in `sensor.cpp`.
* **QoS level** – set `constexpr int QOS` to 0, 1, or 2.
* **TLS & authentication** – edit `main()` in both clients, add `ssl_options` and `user_name` / `password` to the `connect_options` object.
* **Cross‑compiling for Raspberry Pi** – install the same libpaho packages with `apt` on the Pi or use `vcpkg` cross build.

---

## 6. Uninstall

```bash
sudo apt remove --purge libpaho-mqttpp3-dev libpaho-mqtt3as-dev \
  nlohmann-json3-dev mosquitto mosquitto-clients
sudo apt autoremove
```

---

## 7. Troubleshooting

| Symptom                                        | Fix                                                                            |
| ---------------------------------------------- | ------------------------------------------------------------------------------ |
| `Connection refused`                           | Broker not running or wrong host/port. Check `mosquitto -v` output.            |
| `publish error: Unacceptable protocol version` | Broker is ancient (< 1.5). Upgrade Mosquitto.                                  |
| `undefined reference to ...paho...`            | You missed the `-lpaho-mqttpp3 -lpaho-mqtt3as` link flags or the dev packages. |

---

## setup.sh – Automated installer

```bash
#!/usr/bin/env bash
# Installs all dependencies for the MQTT‑sensor demo on Ubuntu 20.04 +.
set -euo pipefail

if [[ "$EUID" -ne 0 ]]; then
  echo "Please run as root: sudo $0" >&2
  exit 1
fi

echo "[1/4] Updating apt cache..." && apt update -y

echo "[2/4] Installing toolchain & libraries..."
apt install -y build-essential cmake git libssl-dev nlohmann-json3-dev \
               libpaho-mqttpp3-dev libpaho-mqtt3as-dev \
               mosquitto mosquitto-clients || true

# Check if Paho packages exist; otherwise build from source
if ! dpkg -s libpaho-mqttpp3-dev &>/dev/null; then
  echo "[3/4] Paho MQTT packages not found – building from source..."
  tempdir=$(mktemp -d)
  pushd "$tempdir" >/dev/null
  git clone --depth 1 https://github.com/eclipse/paho.mqtt.c.git
  cmake -B build -S paho.mqtt.c -DPAHO_WITH_SSL=ON && make -C build -j$(nproc) && make -C build install
  git clone --depth 1 https://github.com/eclipse/paho.mqtt.cpp.git
  cmake -B build -S paho.mqtt.cpp && make -C build -j$(nproc) && make -C build install
  ldconfig
  popd >/dev/null && rm -rf "$tempdir"
fi

echo "[4/4] All dependencies installed. You can now run: cmake -B build && cmake --build build"
```

Save this file as **`setup.sh`**, make it executable (`chmod +x setup.sh`), and run with `sudo ./setup.sh`.

---
