#include "sensors/gnss.hpp"

#include <filesystem>  // needed for runtime_error
#include <string>

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

unsigned long GnssSensor::CRC32Value(int i) {
    int j;

    unsigned long ulCRC;

    ulCRC = i;

    for (j = 8; j > 0; j--) {
        if (ulCRC & 1)

            ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;

        else

            ulCRC >>= 1;
    }

    return ulCRC;
}

unsigned long GnssSensor::CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer) {
    unsigned long ulTemp1;

    unsigned long ulTemp2;

    unsigned long ulCRC = 0;

    while (ulCount-- != 0) {
        ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;

        ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xFF);

        ulCRC = ulTemp1 ^ ulTemp2;
    }

    return (ulCRC);
}

void GnssSensor::read_data(GnssData& data, unsigned char* buffer, serialib& serial) {
    // serial.writeBytes(buffer, 64);
    const char* cmd = "LOG THISPORT BESTPOS ONCE\r\n";
    serial.writeBytes((unsigned char*)cmd, strlen(cmd));
    std::string reads[] = {
        "",//(0) header part 1
        "",//(1) header part 2
        "",//(2) header part 3
        "",//(3) port read from
        "",//(4) sequence number
        "",//(5) reciever idle time %
        "",//(6) time status
        "",//(7) week number
        "",//(8) seconds into week
        "",//(9) reciever status (hex)
        "",//(10) checksum
        "",//(11) reciever software version
        "",//(12) solution status
        "",//(13) solution type
        "",//(14) latitude in degrees
        "",//(15) longitude in degree
        "",//(16) height above ellipsoide in meters
        "",//(17) undulation in meters
        "",//(18) datum
        "",//(19) latitude standard deviation in meters
        "",//(20) longitude standard deviation in meters
        "",//(21) height standard deviation in meters
        "",//(22) base station id
        "",//(23) differential age in seconds
        "",//(24) solution age in seconds
        "",//(25) number of satalites tracked
        "",//(26) number of satalites used in solution
        "",//(27) number of satalites above elevation mask
        "",//(28) number of satalites above mask with L1
        "",//(29) reserved
        "",//(30) extended solution status
        "",//(31) Galileo/BeiDou signal mask
        "",//(32) GPS/GLONASS signal mask
    };
    unsigned char byte;
    int stringSelector = 0;
    while(serial.readChar((char*)&byte, 2000) == 1){
        if(byte == ' '){
            stringSelector += 1;
        }else{
            reads[stringSelector] += byte;
        }
    }
    double lat = std::stod(reads[14]);
    double lon = std::stod(reads[15]);
    std::cout << lat <<", "<< lon << std::endl;
    data.latitude = lat;
    data.longitude = lon;
}

void GnssSensor::sensor_loop() {
    // prepping the command
    unsigned char buffer[64] = {
        0xAA, 0x44, 0x12, 0x1C, 0x01, 0x00, 0x00, 0xC0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00,
        0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    unsigned long lastFour = CalculateBlockCRC32(60, buffer);

    buffer[60] = (lastFour >> 0) & 0xFF;
    buffer[61] = (lastFour >> 8) & 0xFF;
    buffer[62] = (lastFour >> 16) & 0xFF;
    buffer[63] = (lastFour >> 24) & 0xFF;
    // command prepped

    serialib serial;
    char errorOpening = serial.openDevice(SERIAL_PORT_GNSS, 115200);
    // If connection fails, return the error code otherwise, display a success message
    if (errorOpening != 1) {
        std::cout << errorOpening;
    }

    GnssData data = {.latitude = STARTING_LAT, .longitude = STARTING_LONG};

    while (m_running) {
        // get data
        read_data(data, buffer, serial);
        // generate_data(data);

        if (m_callback) {
            m_callback(m_topic, &data, sizeof(data));
        }

        std::this_thread::sleep_for(m_update_interval);
    }
}
