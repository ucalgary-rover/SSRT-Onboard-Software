cmake_minimum_required(VERSION 3.10)
project(HydrogenOzoneSensors)

set(CMAKE_CXX_STANDARD 17)

# Source files (matching manual g++ command)
set(SOURCES
    hydrogen_ozone_sensors.cpp
    geiger_counter.cpp
    IMUDataParse/IMUDataParse.cpp
    IMUDataParse/EasyProfile/EasyProfile.cpp
    IMUDataParse/EasyProfile/EasyObjectDictionary.cpp
    IMUDataParse/EasyProfile/EasyProtocol.cpp
    IMUDataParse/serialib-master/serialib.cpp
)

# Find required packages
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# MQTT (paho.mqtt.cpp)
find_path(PAHO_MQTT_INCLUDE_DIR mqtt/async_client.h)
find_library(PAHO_MQTT_LIB paho-mqttpp3)

# GPSd
find_path(GPSD_INCLUDE_DIR gps.h)
find_library(GPSD_LIB gps)

# Add executable
add_executable(hydrogen_ozone_sensors ${SOURCES})

# Include directories
target_include_directories(hydrogen_ozone_sensors PRIVATE
    ${PAHO_MQTT_INCLUDE_DIR}
    ${GPSD_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    IMUDataParse
    EasyProfile
    serialib-master
    ${nlohmann_json_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(hydrogen_ozone_sensors
    ${PAHO_MQTT_LIB}
    ${GPSD_LIB}
    nlohmann_json::nlohmann_json
    Threads::Threads
)
