#ifndef IMU_H
#include <cstddef>
#define IMU_H
#if defined(_WIN32) || defined(_WIN64)
// for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
// also works for COM0 to COM9.
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea?redirectedfrom=MSDN#communications-resources
#define SERIAL_PORT "\\\\.\\COM3"
#endif
#if defined(__linux__) || defined(__APPLE__)
#define SERIAL_PORT \
    "/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_206D32785242-if00"
using byte = std::byte;
#endif
#include "EasyObjectDictionary.h"
#include "EasyProfile.h"
#include "sensors/sensor_base.hpp"
#include "serialib.h"
// TransducerM communication library instantiation:

class IMUSensor : public SensorBase {
private:
    std::chrono::milliseconds m_update_interval;
    float m_imu_rpy[3];  // Roll, Pitch, Yaw
    float* m_rpy_out;
    EasyObjectDictionary eOD;
    EasyProfile eP;

protected:
    void sensor_loop() override;

public:
    explicit IMUSensor(const std::string& topic, float* m_rpy_out,
                       std::chrono::milliseconds update_interval = std::chrono::milliseconds(500));
    // single read/generate methods that can be called individually as needed
    void read_RPY(float* array,
                  byte* buffer);  // array is a 3 long array, 0 is roll 1 is pitch 2 is yaw.
                                  // buffer is a large char* that will be written over
    void read_MAG(float* array,
                  byte* buffer);  // *array is a 3 long array, 0 is roll 1 is pitch 2
                                  // is yaw, port is which port the IMU is connected to

    float generate_data();

    // methods to be used inside of the read functions
    void RPY_request(float* array, serialib serial, byte* buffer);

    void MAG_request(float* array, serialib serial, byte* buffer);

    void parse_data(float* array, char* rxData, int rxSize);
};

#endif
