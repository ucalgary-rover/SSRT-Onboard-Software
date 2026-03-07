#include "sensors/imu.hpp"

#include <filesystem>  // needed for runtime_error

IMUSensor::IMUSensor(const std::string& topic, std::chrono::milliseconds update_interval, float* m_rpy_out)
    : SensorBase(topic), m_update_interval(update_interval), m_rpy_out(m_rpy_out) {}

float IMUSensor::generate_data() {
    return (std::rand() % 721) - 360.0f;  // generate a random float between -360 and 360
}

void IMUSensor::read_data() {
    throw std::runtime_error(
        "This is a sample class with no real sensor attached. There is no data to be read.");
}

void IMUSensor::sensor_loop() {
    while (m_running) {
        // get data
        for (int i = 0; i < 3; ++i) {
            m_imu_rpy[i] = generate_data();
        }

        // publish data
        if (m_callback) {
            std::string payload = "Roll: " + std::to_string(m_imu_rpy[0]) +
                                  ", Pitch: " + std::to_string(m_imu_rpy[1]) +
                                  ", Yaw: " + std::to_string(m_imu_rpy[2]);
            m_callback(m_topic, payload);
        }

        // wait for more data
        std::this_thread::sleep_for(m_update_interval);
    }
}