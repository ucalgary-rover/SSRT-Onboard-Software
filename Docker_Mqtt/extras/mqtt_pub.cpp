#include <iostream>
#include <fstream>
#include <cstdlib>
#include <nlohmann/json.hpp>

int main() {
    // Open the JSON file
    std::ifstream file("config.json");
    if (!file) {
        std::cerr << "Error: Could not open config.json" << std::endl;
        return 1;
    }

    // Parse the JSON file
    nlohmann::json config;
    file >> config;

    // Extract MQTT parameters
    std::string host = config.value("host", "localhost");
    std::string topic = config.value("topic", "sensor/data");
    std::string message = config.value("message", "Hello, MQTT");

    // Construct the command
    std::string command = "mosquitto_pub -h " + host + " -t \"" + topic + "\" -m \"" + message + "\"";
    
    // Execute the command
    int result = std::system(command.c_str());
    
    // Check if the command executed successfully
    if (result == 0) {
        std::cout << "Message published successfully!" << std::endl;
    } else {
        std::cerr << "Failed to publish message." << std::endl;
    }
    
    return result;
}