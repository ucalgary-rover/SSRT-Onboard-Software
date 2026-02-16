#include "sensors/sensor_base.hpp"

#include <iostream>

SensorBase::~SensorBase() {
    // stop the loop when object is destroyed
    stop();
}

void SensorBase::start(t_sensor_callback callback) {
    if (m_running) {
        // if we are already running, do nothing
        std::cout << "WARNING: Sensor thread is already running" << std::endl;
        return;
    }

    // start the thread
    m_callback = callback;
    m_running = true;
    m_sensor_thread = std::thread(&SensorBase::sensor_loop, this);
}

void SensorBase::stop() {
    if (m_running) {
        m_running = false;
        if (m_sensor_thread.joinable()) {
            m_sensor_thread.join();
        }
    }
}
