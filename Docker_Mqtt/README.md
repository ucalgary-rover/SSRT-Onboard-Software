# Eclipse mosquitto setup 

## Docker installation
Unfamiliar with Docker and Docker setup? See here: https://github.com/ucalgary-rover/SSRT-Docs-Resources/blob/main/resources/HelloDocker.md

### In order to get a working version of eclipse mosquitto, perform the following steps.

#### First we must run the following commands:
Install docker-compose
```bash
sudo apt install docker-compose
```
Install tools for test publisher and subscriber
```bash
sudo apt install mosquitto-clients
```

These commands ensure that you have all the necessary packages installed, plus the docker compose which will allow us to create a docker image.

The following command will create the docker image, I chose to name mine mqtt for simplicity. I have included additional commands to run after this will ensure it is working properly.

Build the image
```bash
Sudo docker-compose up -d
```
Restart the container
```bash
Docker restart <container_name_or_id>
```
Lists all images created
```bash
Docker ps -a
```
Ensure it is working as intended
```bash
docker logs <container_name_or_id> 
```

I have additional steps if this does not work, please reach out to Rodrigo Amesty if that is the case.

When adding publishers and subscribers, run the following commands to get a simple test of how the publishers and subscribers would work. 

You need two terminals to run this.

Run this first in a terminal.
```bash
mosquitto_sub -h localhost -p 1883 -t "test/topic"
```

Then this.
```bash
mosquitto_pub -h localhost -p 1883 -t "test/topic" -m "Hello, MQTT"
```

You should now see the response in subscriber terminal.

### Testing with example-sub.py
This example shows how you could construct a function that can read from the broker in python.

First install paho-mqtt
```bash
pip install paho-mqtt
```

Run the py file with parameters
```bash
python3 example-sub.py -b 127.0.0.1 -p 1883 -t test/topic
```
# Setting Up and Running MQTT Publisher

## 1. Install Essential Libraries
```sh
sudo apt update
sudo apt install build-essential cmake libssl-dev
```

## 2. Clone and Build Paho C and C++ Libraries
```sh
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE
sudo cmake --build build/ --target install
cd ..

git clone https://github.com/eclipse/paho.mqtt.cpp.git
cd paho.mqtt.cpp
cmake -Bbuild -H. -DPAHO_BUILD_STATIC=TRUE -DPAHO_WITH_SSL=TRUE
sudo cmake --build build/ --target install
```

## 3. Install JSON Library
*This step might not work in some cases*
```sh
sudo apt update
sudo apt install nlohmann-json3-dev
```

## 4. Compile the Code
```sh
g++ test_pub.cpp -o test_pub -lpaho-mqttpp3 -lpaho-mqtt3as
```

## 5. Configure the Library (Important Step)
```sh
sudo ldconfig /usr/local/lib
```

## 6. Run the Python Subscriber
```sh
python3 example-sub.py -b localhost -p 1883 -t sensor/data
```

## 7. Run the MQTT Publisher
```sh
./test_pub
```

## Summary
- Installed essential libraries, cloned Paho C and C++ repositories, compiled, and ran the updated code in `test_pub.cpp`.
- **Step 5 (ldconfig) is very important**, as it ensures the Paho and JSON libraries are included without compilation errors.
- Not sure if the Paho libraries need to be cloned for every setup.


