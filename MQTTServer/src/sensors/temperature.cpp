#include "sensors/temperature.hpp"

#include <filesystem>  // needed for runtime_error

TemperatureSensor::TemperatureSensor(const std::string& topic,
                                     std::chrono::milliseconds update_interval)
    : SensorBase(topic), m_update_interval(update_interval) {}

float TemperatureSensor::generate_data() {
    return 20 + (std::rand() % 100) / 10.0f;
}

float TemperatureSensor::read_data() {
    throw std::runtime_error(
        "This is a sample class with no real sensor attached. There is no data to be read.");

    return 0;
}

void TemperatureSensor::sensor_loop() {
    while (m_running) {
        // get data
        float value = generate_data();

        // publish data
        if (m_callback) {
            m_callback(m_topic, std::to_string(value));
        }

        // wait for more data
        std::this_thread::sleep_for(m_update_interval);
    }
}
