cmake_minimum_required(VERSION 3.10)
project(ssrt-onboard-software)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compilation database generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_library(PAHO_MQTTPP3 paho-mqttpp3 REQUIRED)
find_library(PAHO_MQTT3A paho-mqtt3a REQUIRED)

# build dependencies from source where possible
add_subdirectory(dependencies/cpp-dotenv)

set(SOURCES
  src/main.cpp
  src/mqtt_client.cpp
  src/utils.cpp
  src/sensors/sensor_base.cpp
  src/sensors/temperature.cpp
)

# Create the ssrt-onboard-software executable
add_executable(ssrt-onboard-software ${SOURCES})

# Link libraries
target_link_libraries(ssrt-onboard-software
  PRIVATE
    paho-mqttpp3
    paho-mqtt3a
    cpp_dotenv
)

target_include_directories(ssrt-onboard-software
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    /usr/local/include
)
