#include <iostream>
#include <mqtt/async_client.h>
#include <thread>
#include <chrono>

const std::string SERVER_ADDRESS = "tcp://127.0.0.1:1883";  // your broker IP
const std::string CLIENT_ID = "cpp_publisher";
const std::string TOPIC = "sensors/temperature";

int main() {
    mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

    mqtt::connect_options connOpts;
    connOpts.set_clean_session(true);

    try {
        std::cout << "Connecting to MQTT broker..." << std::endl;
        client.connect(connOpts)->wait();

        for (int i = 0; i < 1000; i++) {
            float temp = 20 + (std::rand() % 100) / 10.0f;

            mqtt::message_ptr msg = mqtt::make_message(TOPIC, std::to_string(temp));
            msg->set_qos(1);

            client.publish(msg)->wait();
            std::cout << "Published temperature: " << temp << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        client.disconnect()->wait();
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
