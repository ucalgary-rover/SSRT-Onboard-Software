#ifndef GNSS_H
#define GNSS_H

#include "sensors/sensor_base.hpp"

struct GnssData {
    float latitude;
    float longitude;
};

class GnssSensor : public SensorBase {
protected:
    void sensor_loop() override;

public:
    explicit GnssSensor(const std::string& topic, const std::chrono::milliseconds& update_interval);

    void read_data(GnssData& data);
    void generate_data(GnssData& data);
};

#endif
