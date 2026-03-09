#ifndef IMU_H
#define IMU_H
#if defined (_WIN32) || defined(_WIN64)
    //for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
    //also works for COM0 to COM9.
    //https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea?redirectedfrom=MSDN#communications-resources
    #define SERIAL_PORT "\\\\.\\COM3"
#endif
#if defined (__linux__) || defined(__APPLE__)
    #define SERIAL_PORT "/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_206D32785242-if00"
#endif
#include "sensors/sensor_base.hpp"
#include "dependencies/seriallib/serialib.h"
#include "dependencies/EasyProfile/EasyObjectDictionary.h"
#include "dependencies/EasyProfile/EasyProfile.h"
//TransducerM communication library instantiation:
EasyObjectDictionary eOD;
EasyProfile eP(&eOD);
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
    void read_RPY(float *array, char *buffer); // array is a 3 long array, 0 is roll 1 is pitch 2 is yaw.
                                              // buffer is a large char* that will be written over
    float generate_data();

    // methods to be used inside of the read functions
    void RPY_Request(float *array, serialib serial, char *buffer);

    void Parce_Data(float *array, char* rxData,int rxSize );
};

#endif
