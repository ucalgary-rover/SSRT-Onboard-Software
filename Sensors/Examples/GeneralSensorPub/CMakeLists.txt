cmake_minimum_required(VERSION 3.10)
project(MQTTExample)

# Tell CMake where to find installed libraries
set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})

# Find the Paho MQTT C++ package
find_package(PahoMqttCpp REQUIRED)

# Add executable source
add_executable(main src/main.cpp)

# Link against known libraries
target_link_libraries(main PahoMqttCpp::paho-mqttpp3)
