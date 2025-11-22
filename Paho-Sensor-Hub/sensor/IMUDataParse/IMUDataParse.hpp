#ifndef IMU_DATA_PARSE

// To use the communication library, we need to include the following
// two header files:
#include "EasyProfile/EasyObjectDictionary.h"
#include "EasyProfile/EasyProfile.h"
#include <iostream>
#include <ctime>
#include "serialib-master/serialib.h"
#include <vector>
#include <iomanip>
#include <chrono>
#include <thread>
#if defined(_WIN32) || defined(_WIN64)
// for serial ports above "COM9", we must use this extended syntax of "\\.\COMx".
// also works for COM0 to COM9.
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea?redirectedfrom=MSDN#communications-resources
#define SERIAL_PORT "\\\\.\\COM9"
#endif
#if defined(__linux__) || defined(__APPLE__)
#define SERIAL_PORT "/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_Comport_206D32785242-if00"
#endif

// to compile
// g++ IMUDataParse.cpp EasyProfile/EasyProfile.cpp EasyProfile/EasyObjectDictionary.cpp EasyProfile/EasyProtocol.cpp serialib-master/serialib.cpp -o IMUDataParse

void IMU_RPY(float *array, int port, char *buffer);

#endif