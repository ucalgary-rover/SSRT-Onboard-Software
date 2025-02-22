#include <iostream>
#include <cstdlib>
#include <nlohmann/json.hpp>

int main() {
    // Define the command
    std::string command = "mosquitto_pub -h localhost -t \"sensor/data\" -m \"Hello, MQTT\"";
    
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