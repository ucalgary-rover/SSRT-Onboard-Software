#include <getopt.h>
#include <mqtt/async_client.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <thread>
#include <vector>
#include <optional>

#include "dotenv.h"
#include "mqtt_client.hpp"
#include "sensors/gnss.hpp"
#include "sensors/imu.hpp"
#include "sensors/sensor_base.hpp"
#include "sensors/temperature.hpp"

// used for sending disconnect request in signal handler
MQTTClient* global_mqtt_client = nullptr;
// make sure only one thread is writing at a time
std::mutex cout_mutex;
std::atomic<bool> g_debug_mode{false};

std::optional<std::filesystem::path> find_default_env_path() {
    namespace fs = std::filesystem;
    const std::array<fs::path, 2> directories = {fs::current_path(), fs::current_path().parent_path()};
    for (const auto& directory : directories) { 
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            continue;
        }
        for (const auto& current : fs::directory_iterator(directory)) {
            if (!current.is_regular_file()) {
                continue;
            }
            const auto& path = current.path();
            // checks the current directory and one level up for any .env file
            if (path.extension() == ".env") {
                return path;
            }
        }
    }
    return std::nullopt;
}

void signal_handler(int signum) {
    std::cout << "\nShutdown signal received" << std::endl;
    if (global_mqtt_client) {
        global_mqtt_client->request_shutdown();
    }
}

void handle_cmd_line_args(int argc, char* argv[], std::string& env_file_path, bool& debug_mode) {
    static struct option long_options[] = {
        {"env-file-path", required_argument, nullptr, 'e'},
        {"debug", no_argument, nullptr, 'd'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0},
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "e:dh", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'e':
                std::cout << "HERE!!" << std::endl;
                env_file_path = optarg;
                break;
            case 'd':
                debug_mode = true;
                std::cout << "Debug mode enabled\n";
                break;
            case 'h':
                std::cout << "Usage: " << argv[0] << " [OPTIONS]\n\n"
                          << "Options:\n"
                          << "  -e, --env-file-path <path>  Path to the environment file (default: any .env file found in the current or parent directory)\n"
                          << "  -d, --debug                 Use simulated data\n"
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
    std::string env_file_path;
    bool debug_mode = false;
    handle_cmd_line_args(argc, argv, env_file_path, debug_mode);
    // if no argument is provided for env file or the file is not found, look for one to use
    if(env_file_path.empty() || !std::filesystem::exists(env_file_path)) {
        auto found = find_default_env_path();
        if(!found) {
            throw std::runtime_error(".env file not found in current or parent directory");
        }
        env_file_path = found->string();
    }
    g_debug_mode.store(debug_mode);
    std::cout << "Finished command line stuff. Using env file: " << env_file_path << std::endl;

    // load environment variables
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
                                                              std::chrono::milliseconds(500)));
        sensors.push_back(
            std::make_unique<IMUSensor>(env_values["IMU_TOPIC"], std::chrono::milliseconds(100)));
        sensors.push_back(
            std::make_unique<GnssSensor>(env_values["GNSS_TOPIC"], std::chrono::milliseconds(500)));

        // start all sensors
        for (auto& sensor : sensors) {
            sensor->start(publish_to_mqtt_callback);
        }

        // loop to keep main thread alive,
        // just waiting for shutdown signal
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
