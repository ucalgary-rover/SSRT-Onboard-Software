#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <mqtt/async_client.h>

const std::string CONFIG_FILE {"./files/config.json"};

int main() {
    try {
        // Open and parse the JSON file
        std::ifstream file(CONFIG_FILE);
        if (!file) {
            std::cerr << "Error: Could not open " << CONFIG_FILE << std::endl;
            return 1;
        }

        nlohmann::json config;
        file >> config;

        // Extract MQTT parameters from JSON
        std::string server_address = config.value("server_address", "tcp://localhost:1883");
        std::string client_id = config.value("client_id", "paho_publisher");
        std::string topic = config.value("topic", "sensor/data");
        std::string payload = config.value("message", "Hello, MQTT");

        mqtt::async_client client(server_address, client_id);
        mqtt::connect_options connOpts;
        connOpts.set_clean_session(true);

        std::cout << "Connecting to the MQTT broker..." << std::endl;
        client.connect(connOpts)->wait();

        mqtt::message msg(topic, payload, 1, false);

        std::cout << "Publishing message..." << std::endl;
        client.publish(mqtt::make_message(topic, payload, 1, false))->wait();

        std::cout << "Message published successfully!" << std::endl;

        client.disconnect()->wait();
    } catch (const mqtt::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}