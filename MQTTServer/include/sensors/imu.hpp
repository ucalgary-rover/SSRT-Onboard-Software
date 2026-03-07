#ifndef IMU_H
#define IMU_H

#include "sensors/sensor_base.hpp"

class IMUSensor : public SensorBase {
private:
    std::chrono::milliseconds m_update_interval;
    float m_imu_rpy[3]; // Roll, Pitch, Yaw
    float* m_rpy_out;

protected:
    void sensor_loop() override;

public:
    explicit IMUSensor(const std::string& topic, std::chrono::milliseconds update_interval =
                                                        std::chrono::milliseconds(500), float* m_rpy_out);

    // single read/generate methods that can be called individually as needed
    void read_data();
    float generate_data();
};

#endif