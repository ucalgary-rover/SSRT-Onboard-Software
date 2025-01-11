#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <stdexcept>
#include "serial/serial.h"

// UBXData struct to store parsed GNSS data
struct UBXData {
    double latitude = 0.0;     // Latitude in degrees
    double longitude = 0.0;    // Longitude in degrees
    double speed = 0.0;        // Speed in m/s
    double altitude = 0.0;     // Altitude above sea level in meters
    std::string day;           // Date in "YYYY-MM-DD" format
    uint64_t time = 0;         // Time in milliseconds
    bool valid = false;        // Indicates whether the data is valid
};

// Parse UBX data (NAV-PVT messages only)
UBXData parseUBXData(const std::vector<uint8_t>& raw_data) {
    UBXData data;

    // Check minimum message size
    if (raw_data.size() < 8) {
        std::cerr << "Invalid UBX message: too short\n";
        data.valid = false;
        return data;
    }

    // Verify UBX sync characters
    if (raw_data[0] != 0xB5 || raw_data[1] != 0x62) {
        std::cerr << "Invalid UBX message: incorrect sync characters\n";
        data.valid = false;
        return data;
    }

    // Extract message class, ID, and payload length
    uint8_t msgClass = raw_data[2];
    uint8_t msgID = raw_data[3];
    uint16_t payloadLength = raw_data[4] | (raw_data[5] << 8);

    // Check for complete payload
    if (raw_data.size() < 6 + payloadLength + 2) {
        std::cerr << "Invalid UBX message: incomplete payload\n";
        data.valid = false;
        return data;
    }

    // Extract the payload
    const uint8_t* payload = &raw_data[6];

    // Handle NAV-PVT message
    if (msgClass == 0x01 && msgID == 0x07) {
        if (payloadLength < 92) {
            std::cerr << "Invalid NAV-PVT message: payload too short\n";
            data.valid = false;
            return data;
        }

        uint8_t fixType = payload[20];
        if (fixType < 3) { // Fix type < 3 means no reliable 3D fix
            std::cerr << "UBX NAV-PVT: No valid GNSS fix (fixType=" << (int)fixType << ")\n";
            data.valid = false;
            return data;
        }

        data.time = *(reinterpret_cast<const uint32_t*>(&payload[0])); // iTOW
        int32_t lat = *(reinterpret_cast<const int32_t*>(&payload[28])); // Latitude (1e-7 degrees)
        int32_t lon = *(reinterpret_cast<const int32_t*>(&payload[24])); // Longitude (1e-7 degrees)
        int32_t height = *(reinterpret_cast<const int32_t*>(&payload[36])); // Altitude (mm)
        uint32_t speed = *(reinterpret_cast<const uint32_t*>(&payload[60])); // Ground speed (mm/s)

        uint16_t year = *(reinterpret_cast<const uint16_t*>(&payload[4]));
        uint8_t month = payload[6];
        uint8_t day = payload[7];

        data.latitude = lat * 1e-7;         // Convert to degrees
        data.longitude = lon * 1e-7;       // Convert to degrees
        data.altitude = height * 1e-3;     // Convert to meters
        data.speed = speed * 1e-3;         // Convert to m/s
        data.day = std::to_string(year) + "-" +
                   (month < 10 ? "0" : "") + std::to_string(month) + "-" +
                   (day < 10 ? "0" : "") + std::to_string(day);

        data.valid = true;
    } else {
        std::cerr << "Unsupported UBX message: Class " << std::hex << (int)msgClass
                  << " ID " << (int)msgID << "\n";
        data.valid = false;
    }

    return data;
}

// Parse NMEA messages
bool parseNMEA(const std::string& nmea, UBXData& data) {
    if (nmea.empty() || nmea[0] != '$') {
        return false;
    }

    std::istringstream iss(nmea);
    std::string token;
    std::vector<std::string> fields;
    while (std::getline(iss, token, ',')) {
        fields.push_back(token);
    }

    if (fields[0] == "$GNGGA") {
        // GGA: Global Positioning System Fix Data
        if (fields.size() > 6 && fields[6] != "0") { // Check fix quality
            if (!fields[2].empty()) {
                data.latitude = std::stod(fields[2]) / 100.0;
                if (fields[3] == "S") data.latitude = -data.latitude;
            }
            if (!fields[4].empty()) {
                data.longitude = std::stod(fields[4]) / 100.0;
                if (fields[5] == "W") data.longitude = -data.longitude;
            }
            if (!fields[9].empty()) {
                data.altitude = std::stod(fields[9]);
            }
            data.valid = true;
            return true;
        } else {
            std::cerr << "No GNSS fix in GGA message.\n";
            return false;
        }

    } else if (fields[0] == "$GNRMC") {
        // RMC: Recommended Minimum Specific GNSS Data
        if (fields.size() > 2 && fields[2] == "A") { // Check status (A = Active)
            if (!fields[3].empty()) {
                data.latitude = std::stod(fields[3]) / 100.0;
                if (fields[4] == "S") data.latitude = -data.latitude;
            }
            if (!fields[5].empty()) {
                data.longitude = std::stod(fields[5]) / 100.0;
                if (fields[6] == "W") data.longitude = -data.longitude;
            }
            if (!fields[7].empty()) {
                data.speed = std::stod(fields[7]) * 0.51444; // Convert knots to m/s
            }
            data.valid = true;
            return true;
        } else {
            std::cerr << "No GNSS fix in RMC message.\n";
            return false;
        }

    } else if (fields[0] == "$GNVTG") {
        // VTG: Course Over Ground and Ground Speed
        if (fields.size() > 7 && !fields[7].empty()) {
            data.speed = std::stod(fields[7]) * 0.51444; // Convert knots to m/s
            data.valid = true;
            return true;
        } else {
            std::cerr << "VTG message does not contain valid ground speed.\n";
            return false;
        }

    } else if (fields[0] == "$GNGLL") {
        // GLL: Geographic Position - Latitude/Longitude
        if (fields.size() > 6 && fields[6] == "A") { // Check status (A = Active)
            if (!fields[1].empty()) {
                data.latitude = std::stod(fields[1]) / 100.0;
                if (fields[2] == "S") data.latitude = -data.latitude;
            }
            if (!fields[3].empty()) {
                data.longitude = std::stod(fields[3]) / 100.0;
                if (fields[4] == "W") data.longitude = -data.longitude;
            }
            data.valid = true;
            return true;
        } else {
            std::cerr << "GLL message indicates invalid position.\n";
            return false;
        }

    } else if (fields[0] == "$GNGSA") {
        // GSA: GNSS DOP and Active Satellites
        if (fields.size() > 17) {
            std::cout << "GNGSA Message:\n";
            std::cout << "  PDOP: " << fields[15] << "\n";
            std::cout << "  HDOP: " << fields[16] << "\n";
            std::cout << "  VDOP: " << fields[17] << "\n";
            return true;
        } else {
            std::cerr << "Incomplete GSA message.\n";
            return false;
        }

    } else if (fields[0].find("GSV") != std::string::npos) {
        // GSV: GNSS Satellites in View
        if (fields.size() > 3) {
            int numSV = std::stoi(fields[3]);
            std::cout << "GSV Message:\n";
            std::cout << "  Satellites in View: " << numSV << "\n";
            return true;
        } else {
            std::cerr << "Incomplete GSV message.\n";
            return false;
        }

    } else {
        std::cerr << "Unsupported NMEA message type: " << fields[0] << "\n";
        return false;
    }
}


// GNSS data reading loop
void readGNSSData(serial::Serial& serial_port, double period, UBXData& last_valid_data) {
    std::vector<uint8_t> buffer;

    while (true) {
        try {
            if (serial_port.available()) {
                std::vector<uint8_t> incoming(serial_port.available());
                serial_port.read(incoming.data(), incoming.size());

                buffer.insert(buffer.end(), incoming.begin(), incoming.end());

                while (!buffer.empty()) {
                    size_t message_end = std::find(buffer.begin(), buffer.end(), '\n') - buffer.begin();
                    if (message_end == buffer.size()) {
                        break; // No complete message yet
                    }

                    std::vector<uint8_t> message(buffer.begin(), buffer.begin() + message_end + 1);
                    buffer.erase(buffer.begin(), buffer.begin() + message_end + 1);

                    if (message.size() >= 2 && message[0] == 0xB5 && message[1] == 0x62) {
                        UBXData current_data = parseUBXData(message);
                        if (current_data.valid) {
                            last_valid_data = current_data;
                            std::cout << "\nUBX Data:\n";
                            std::cout << "  Latitude: " << current_data.latitude << "\n";
                            std::cout << "  Longitude: " << current_data.longitude << "\n";
                            std::cout << "  Altitude: " << current_data.altitude << " meters\n";
                            std::cout << "  Speed: " << current_data.speed << " m/s\n";
                            std::cout << "  Date: " << current_data.day << "\n";
                        }
                    } else if (!message.empty() && message[0] == '$') {
                        std::string nmea_message(message.begin(), message.end());
                        UBXData current_data;
                        if (parseNMEA(nmea_message, current_data)) {
                            last_valid_data = current_data;
                            std::cout << "\nNMEA Data:\n";
                            std::cout << "  Latitude: " << current_data.latitude << "\n";
                            std::cout << "  Longitude: " << current_data.longitude << "\n";
                        }
                    } else {
                        std::cerr << "Unknown message type received.\n";
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(period * 1000)));
    }
}

// Main function
int main() {
    const std::string GNSS_PORT = "/dev/tty.usbmodem1301"; // Adjust for your GNSS port
    const unsigned int GNSS_BAUDRATE = 38400;
    const double GNSS_PERIOD = 0.5;

    try {
        serial::Serial serial_port(GNSS_PORT, GNSS_BAUDRATE, serial::Timeout::simpleTimeout(1000));
        if (!serial_port.isOpen()) {
            throw std::runtime_error("Failed to open serial port.");
        }

        UBXData last_valid_data;
        readGNSSData(serial_port, GNSS_PERIOD, last_valid_data);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
