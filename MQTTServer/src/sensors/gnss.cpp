#include "sensors/gnss.hpp"

#include <filesystem>  // needed for runtime_error

#define STARTING_LAT 51.45404
#define STARTING_LONG -112.67683

GnssSensor::GnssSensor(const std::string& topic, const std::chrono::milliseconds& update_interval)
    : SensorBase(topic, update_interval) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}


void GnssSensor::generate_data(GnssData& data) {
    data.latitude += -0.001 + (rand() / (double)RAND_MAX) * 0.002;
    data.longitude += -0.001 + (rand() / (double)RAND_MAX) * 0.002;
}

unsigned long CRC32Value(int i) {

     int j;

     unsigned long ulCRC;

     ulCRC = i;

     for ( j = 8 ; j > 0; j-- ) {

          if ( ulCRC & 1 )

               ulCRC = ( ulCRC >> 1 ) ^ CRC32_POLYNOMIAL;

          else

               ulCRC >>= 1;

     }

     return ulCRC;

} 

unsigned long CalculateBlockCRC32( unsigned long ulCount, unsigned char *ucBuffer ) {

     unsigned long ulTemp1;

     unsigned long ulTemp2;

     unsigned long ulCRC = 0;

     while ( ulCount-- != 0 ) {

          ulTemp1 = ( ulCRC >> 8 ) & 0x00FFFFFFL;

          ulTemp2 = CRC32Value( ((int) ulCRC ^ *ucBuffer++ ) & 0xFF );

          ulCRC = ulTemp1 ^ ulTemp2;

     }

     return( ulCRC );

} 

void read_data(GnssData& data, unsigned char* buffer, serialib& serial){

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

    data.latitude = lat;
    data.longitude = lon;
}

void GnssSensor::sensor_loop() {
    // prepping the command
     unsigned char buffer[64] = {
        0xAA, 0x44, 0x12, 0x1C, 0x01, 0x00, 0x00, 0xC0, 0x20, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00,
        0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    unsigned long lastFour = CalculateBlockCRC32(60, buffer);

    buffer[60] = (lastFour >> 0 ) & 0xFF;
    buffer[61] = (lastFour >> 8 ) & 0xFF;
    buffer[62] = (lastFour >> 16 ) & 0xFF;
    buffer[63] = (lastFour >> 24 ) & 0xFF;
    // command prepped

    GnssData data = {.latitude = STARTING_LAT, .longitude = STARTING_LONG};

    while (m_running) {
        // get data
        // read_data(data);
        generate_data(data);

        if (m_callback) {
            m_callback(m_topic, &data, sizeof(data));
        }

        std::this_thread::sleep_for(m_update_interval);
    }
}
