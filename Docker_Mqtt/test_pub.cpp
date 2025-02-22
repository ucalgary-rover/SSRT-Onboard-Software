#include <iostream>
#include <mqtt/async_client.h>

const std::string SERVER_ADDRESS {"tcp://localhost:1883"};
const std::string CLIENT_ID {"paho_publisher"};
const std::string TOPIC {"sensor/data"};

int main() {
    try {
        mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

        mqtt::connect_options connOpts;
        connOpts.set_clean_session(true);

        std::cout << "Connecting to the MQTT broker..." << std::endl;
        client.connect(connOpts)->wait();

        std::string payload = "Hello, MQTT";
        mqtt::message msg(TOPIC, payload, 1, false);

        std::cout << "Publishing message..." << std::endl;
        client.publish(mqtt::make_message(TOPIC, payload, 1, false))->wait();

        std::cout << "Message published successfully!" << std::endl;

        client.disconnect()->wait();
    } catch (const mqtt::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
