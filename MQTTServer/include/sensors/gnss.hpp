#ifndef GNSS_H
#define GNSS_H

#define CRC32_POLYNOMIAL 0xEDB88320L
#include "sensors/sensor_base.hpp"
#include "serialib.h"
#include <vector>

#if defined(__linux__) || defined(__APPLE__)
    #define SERIAL_PORT \
        "/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_206D32785242-if00" //THIS IS NOT CORRECT
    #endif

struct GnssData {
    float latitude;
    float longitude;
};

class GnssSensor : public SensorBase {
protected:
    void sensor_loop() override;
   
public:
    explicit GnssSensor(const std::string& topic, const std::chrono::milliseconds& update_interval);
    void read_data(GnssData& data, unsigned char* buffer, serialib& serial);
    void generate_data(GnssData& data);
    unsigned long CRC32Value(int i);
    unsigned long CalculateBlockCRC32( unsigned long ulCount, unsigned char *ucBuffer );
};

#endif