#include "serialib.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <cstring>

#define SERIAL_PORT "/dev/serial/by-id/usb-STMicroelectronics_STM32_Virtual_ComPort_206D32785242-if00"

int main() {
    serialib serial;
    int errorOpening = serial.openDevice(SERIAL_PORT, 115200);
    if (errorOpening != 1) {
        std::cout << "open error" << std::endl;
        return -1;
    }

    unsigned char buffer[] = {
        0xAA, 0x44, 0x12, 0x1C, 0x01, 0x00, 0x00, 0xC0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00,
        0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xB5, 0x84, 0x89, 0x90
    };

    serial.writeBytes(buffer, sizeof(buffer));

    // Read byte by byte, looking for AA 44 12 sync bytes to find
    // the start of the binary response, then read the rest of the
    // message based on the header's reported length
    std::vector<unsigned char> response;
    unsigned char byte;
    bool synced = false;
    int syncCount = 0;

    // Step 1: hunt for the AA 44 12 sync sequence
    while (!synced) {
        if (serial.readChar((char*)&byte, 2000) != 1) {
            std::cout << "timeout waiting for sync" << std::endl;
            return -1;
        }

        if (byte == 0xAA) {
            response.clear();
            response.push_back(byte);
            syncCount = 1;
        } else if (syncCount == 1 && byte == 0x44) {
            response.push_back(byte);
            syncCount = 2;
        } else if (syncCount == 2 && byte == 0x12) {
            response.push_back(byte);
            synced = true;
        } else {
            syncCount = 0;
            response.clear();
        }
    }

    // Step 2: read the rest of the header (25 more bytes, total header = 28)
    for (int i = 0; i < 25; i++) {
        if (serial.readChar((char*)&byte, 2000) != 1) {
            std::cout << "timeout reading header" << std::endl;
            return -1;
        }
        response.push_back(byte);
    }

    // Step 3: extract message body length from header bytes 8-9 (little-endian Ushort)
    // offset 8 in the header = index 8 in response vector
    uint16_t messageLength = response[8] | (response[9] << 8);

    // Step 4: read the body + 4 CRC bytes
    int remaining = messageLength + 4;
    for (int i = 0; i < remaining; i++) {
        if (serial.readChar((char*)&byte, 2000) != 1) {
            std::cout << "timeout reading body" << std::endl;
            return -1;
        }
        response.push_back(byte);
    }

    // Print the full captured response as hex
    std::cout << "Response (" << std::dec << response.size() << " bytes):" << std::endl;
    for (size_t i = 0; i < response.size(); i++) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)response[i] << " ";
        if ((i + 1) % 16 == 0) std::cout << std::endl;
    }
    std::cout << std::endl;
    double lat, lon;
    memcpy(&lat, response.data() + 36, 8);
    memcpy(&lon, response.data() + 44, 8);

    std::cout << std::fixed << std::setprecision(9);
    std::cout << "Latitude:  " << lat << std::endl;
    std::cout << "Longitude: " << lon << std::endl;
    return 0;
}