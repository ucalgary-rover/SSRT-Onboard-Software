#include <mqtt/async_client.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>      // sudo apt install nlohmann-json3-dev

using json = nlohmann::json;      // easier this way, trust

const std::string BROKER   = "tcp://127.0.0.1:1883";  // keep one for all sensors
const std::string CLIENTID = "sensor_1";            // We should have unique IDs for each sensors
const std::string TOPIC    = "sensors/sensor_1";    // Indivivual topics for each of the sensors
const auto INTERVAL = std::chrono::seconds(2);
constexpr int QOS = 1;  // Quality of service, use 1 for atleast once

struct Reading {
/*
Have whatever you need from the sensors here
*/
};

Reading readSensors() {
    // Feel free to put the sensor read here
}


int main() {
    mqtt::async_client cli(BROKER, CLIENTID);
    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        cli.connect(connOpts)->wait();
    std::cout << "Connected to broker\n";

    while (true) {
        // Call readSensors() here


        // Now package it into a json object (an example is written below for making and publishing it)
        /* json payload = {
            {"temperature", r.temperature},
            {"humidity",    r.humidity},
            {"ts",          std::time(nullptr)}
        };
        std::string body = payload.dump();
        */

        // Now we have to publish to the broker itself, I have written up a small example code, try this

        /* try {
            cli.publish(TOPIC, body.c_str(), body.size(), QOS, false)->wait();
            std::cout << "Published: " << body << '\n';
        } catch (const mqtt::exception& e) {
            std::cerr << "Publish error: " << e.what() << '\n';
        }

        std::this_thread::sleep_for(INTERVAL);
        */


    }

    } catch (const mqtt::exception& e) {
        std::cerr << "MQTT error: " << e.what() << '\n';
    }
    
    cli.disconnect()->wait();
    return 0;
}
