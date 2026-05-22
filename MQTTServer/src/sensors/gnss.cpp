#include "sensors/gnss.hpp"

#include <filesystem>  // needed for runtime_error

#define STARTING_LAT 51.45404
#define STARTING_LONG -112.67683

GnssSensor::GnssSensor(const std::string& topic, const std::chrono::milliseconds& update_interval)
    : SensorBase(topic, update_interval) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void GnssSensor::read_data(GnssData& data) {
    throw std::runtime_error("GNSS data read has not been implemented yet!!!");
}

void GnssSensor::generate_data(GnssData& data) {
    data.latitude += -0.001 + (rand() / (double)RAND_MAX) * 0.002;
    data.longitude += -0.001 + (rand() / (double)RAND_MAX) * 0.002;
}

void GnssSensor::sensor_loop() {
    GnssData data = {.latitude = STARTING_LAT, .longitude = STARTING_LONG};

    while (m_running) {
        // get data
        generate_data(data);

        if (m_callback) {
            m_callback(m_topic, &data, sizeof(data));
        }

        std::this_thread::sleep_for(m_update_interval);
    }
}
