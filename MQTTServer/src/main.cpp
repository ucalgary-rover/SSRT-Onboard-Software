#include <getopt.h>
#include <mqtt/async_client.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>

#include "dotenv.h"
#include "mqtt_client.hpp"
#include "sensors/sensor_base.hpp"
#include "sensors/temperature.hpp"

#define DEFAULT_ENV_FILE_PATH "config.env"

// used for sending disconnect request in signal handler
MQTTClient* global_mqtt_client = nullptr;
// make sure only one thread is writing at a time
std::mutex cout_mutex;

void signal_handler(int signum) {
    std::cout << "\nShutdown signal received" << std::endl;
    if (global_mqtt_client) {
        global_mqtt_client->request_shutdown();
    }
}

void handle_cmd_line_args(int argc, char* argv[], const char*& env_file_path) {
    static struct option long_options[] = {
        {"env-file-path", required_argument, nullptr, 'e'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0},
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "e:h", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'e':
                std::cout << "HERE!!" << std::endl;
                env_file_path = optarg;
                break;
            case 'h':
                std::cout << "Usage: " << argv[0] << " [OPTIONS]\n\n"
                          << "Options:\n"
                          << "  -e, --env-file-path <path>  Path to the environment file (default: "
                          << DEFAULT_ENV_FILE_PATH << ")\n"
                          << "  -h, --help                  Show this help message\n";
                exit(0);
            case '?':
                std::cerr << "Unknown option. Use -h or --help for help." << std::endl;
                exit(1);
        }
    }
}

int main(int argc, char* argv[]) {
    // register signal handlers
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // termination request

    // handle command line arguments
    const char* env_file_path = DEFAULT_ENV_FILE_PATH;
    handle_cmd_line_args(argc, argv, env_file_path);
    std::cout << "Finished command line stuff. " << env_file_path << std::endl;

    // load environment variables
    // TODO: error handling if this file is not found
    if (!std::filesystem::exists(env_file_path)) {
        throw std::runtime_error("config.env file not found");
    }
    auto& env_values = dotenv::env.load_dotenv(env_file_path);

    try {
        MQTTClient mqtt(env_values["SERVER_ADDRESS"], env_values["CLIENT_ID"]);
        global_mqtt_client = &mqtt;

        mqtt.connect();

        // callback function for publishing from sensor threads
        auto publish_to_mqtt_callback = [&mqtt](const std::string& topic, const void* payload,
                                                std::size_t length) {
            try {
                mqtt.publish(topic, payload, length);
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "Published " << length << " bytes to " << topic << std::endl;
            } catch (const std::exception& e) {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cerr << "Publish error: " << e.what() << std::endl;
            }
        };

        // create vector to hold the different sensors
        std::vector<std::unique_ptr<SensorBase>> sensors;

        sensors.push_back(std::make_unique<TemperatureSensor>(env_values["TEMPERATURE_TOPIC"],
                                                              std::chrono::seconds(1)));

        // start all sensors
        for (auto& sensor : sensors) {
            sensor->start(publish_to_mqtt_callback);
        }

        // loop to keep main thread alive, just waiting for shutdown signal
        while (!mqtt.should_shutdown()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // stop all sensors
        std::cout << "Stopping sensors" << std::endl;
        for (auto& sensor : sensors) {
            sensor->stop();
        }

        mqtt.disconnect();
    } catch (const mqtt::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
