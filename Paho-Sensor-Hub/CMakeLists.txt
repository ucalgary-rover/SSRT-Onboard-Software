cmake_minimum_required(VERSION 3.14)
project(sensor_hub)

add_executable(hydrogen_ozone_sensors
    sensor/hydrogen_ozone_sensors.cpp)

target_link_libraries(hydrogen_ozone_sensors
    PRIVATE
        paho-mqttpp3
        paho-mqtt3as)

add_executable(processor driver/processor.cpp)
target_link_libraries(processor
    PRIVATE
        paho-mqttpp3
        paho-mqtt3as)

target_include_directories(hydrogen_ozone_sensors PRIVATE /user/include)
target_include_directories(processor PRIVATE /user/include)