#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include "sensors/sensor_base.hpp"

class TemperatureSensor : public SensorBase {
private:
    std::chrono::milliseconds m_update_interval;

protected:
    void sensor_loop() override;

public:
    explicit TemperatureSensor(const std::string& topic, std::chrono::milliseconds update_interval =
                                                             std::chrono::milliseconds(500));

    // single read/generate methods that can be called individually as needed
    float read_data();
    float generate_data();
};

#endif
