#include "sensors/imu.hpp"

#include <filesystem>  // needed for runtime_error
#include <cmath>
IMUSensor::IMUSensor(const std::string& topic, float* m_rpy_out,
                     std::chrono::milliseconds update_interval)
    : SensorBase(topic,update_interval),
      m_rpy_out(m_rpy_out),
      eOD(EasyObjectDictionary()),
      eP(&eOD) {}

float IMUSensor::random_value(float max, float min) {
    return (std::rand() / (float)RAND_MAX) * (max - min) + min;
}

void IMUSensor::generate_data(IMUData& data) {
    data.roll = random_value(180.0f, -180.0f);
    data.yaw = random_value(180.0f, -180.0f);
    data.pitch = random_value(180.0f, -180.0f);
    data.battery_temp = random_value(60.0f, 20.0f);
    data.power = random_value(100.0f, 50.0f);
    data.heading_deg = 0;
    data.speed = 0;
}

// void IMUSensor::read_RPY(float* array, byte* buffer) {
//     serialib serial;
//     char errorOpening = serial.openDevice(SERIAL_PORT, 115200);
//     // If connection fails, return the error code otherwise, display a success message
//     if (errorOpening != 1) {
//         std::cout << errorOpening;
//     }
//     RPY_request(serial, buffer);
//     serial.closeDevice();
// }

// void IMUSensor::read_MAG(float* array, byte* buffer) {
//     serialib serial;
//     char errorOpening = serial.openDevice(SERIAL_PORT, 115200);
//     // If connection fails, return the error code otherwise, display a success message
//     if (errorOpening != 1) {
//         std::cout << errorOpening;
//     }
//     MAG_request(IMUData& data, serial, buffer);
//     serial.closeDevice();
// }

// I am keeping these in case the new function doesn't work

void IMUSensor::parse_data(
                           IMUData& data,  ///<[OUTPUT] Pointer to the data struct
                           char* rxData,  ///< [INPUT] Pointer to the RX data array
                           int rxSize     ///< [INPUT] Size of the RX data array
) {
    Ep_Header header;
    if (EP_SUCC_ ==
        eP.On_RecvPkg(rxData, rxSize,
                      &header)) {  // Step 2: Tell the library that new data has arrived.
                                   // It does not matter if the new data only contains a fraction
                                   // of a complete data package, nor does it matter if the data
                                   // stream is corrupted during the transmission. On_RecvPkg() will
                                   // only return EP_SUCC_ when a complete and correct package has
                                   // arrived. Example Reading of the Short ID of the device who
                                   // sends the data:
        switch (
            header.cmd) {  // Step 3.2: header.cmd tells what kind of data is inside the payload.

            case EP_CMD_RPY_: {
                Ep_RPY ep_RPY;
                if (EP_SUCC_ ==
                    eOD.Read_Ep_RPY(
                        &ep_RPY)) {  // Another Example reading of the received Roll Pitch and Yaw
                    float roll = ep_RPY.roll;
                    float pitch = ep_RPY.pitch;
                    float yaw = ep_RPY.yaw;

                    //					std::cout<<"roll"<<roll<<"\npitch"<<pitch<<"\nyaw"<<yaw;//use
                    // data as you wish
                    data.roll = roll;
                    data.pitch = pitch;
                    data.yaw = yaw;
                }
            } break;
            case EP_CMD_Q_S1_E_:{
                Ep_Q_s1_e quaternion;
                float q0 = quaternion.q[0];
                float q1 = quaternion.q[1];
                float q2 = quaternion.q[2];
                float q3 = quaternion.q[3];
                data.heading_deg = atan2(2*(q0*q3+q1*q2),1-2*(q2*q2+q3*q3)) * 180/M_PI;
            }case EP_CMD_Raw_GYRO_ACC_MAG_: {
                Ep_Raw_GyroAccMag ep_raw;
                if (EP_SUCC_ == eOD.Read_Ep_Raw_GyroAccMag(&ep_raw)) {
                    data.acc_x = ep_raw.acc[0] * 9.81;
                    data.acc_y = ep_raw.acc[1] * 9.81;
                    data.acc_z = ep_raw.acc[2] * 9.81;
                    std::cout <<"testing accelerometer\nx:"<<data.acc_x<<"\ny:"<<data.acc_y<<"\nz:"<<data.acc_z;
                }
            }break;
            default: {
                std::cout << "wrong data";
                break;
            }
        }
    } else {
        std::cout << "broken data\n";
    }
}

void IMUSensor::RPY_request(IMUData& data, serialib& serial) {
    //    uint16 toId = ui->spinBox_TX_Request_ShortId->value();     // Step 1: Get the destination
    //    device ID to which the request is sent
    uint16 toId = EP_ID_BROADCAST_;  //       You can also choose to boardcast the request without
                                     //       specifying the target device ID
    if (EP_SUCC_ ==
        eOD.Write_Ep_Request(toId,
                             EP_CMD_RPY_)) {  // Step 2: Write the device ID into the data structure
                                              // (i.e. the request itself) pending to be sent
        EP_ID_TYPE_ txToId;
        char* txPkgData;
        int txPkgSize;
        EP_CMD_TYPE_ txCmd =
            EP_CMD_RPY_;  // Step 3: Define what type of data we are requesting for. (in this
                          // example, we are requesting Roll-Pitch-Yaw readings.
        if (EP_SUCC_ == eP.On_SendPkg(txCmd, &txToId, &txPkgData,
                                      &txPkgSize)) {  // Step 4: create a package out of the data
                                                      // structure (i.e. the payload) to be sent
            serial.writeBytes(txPkgData, txPkgSize);  // Step 5:  Send the package via Serial Port
        }
    }    
    char buffer[255];
    serial.readBytes(buffer, 255, 200);
    parse_data(data, buffer, 255);
}

void IMUSensor::MAG_request(IMUData& data, serialib& serial) {
    //    uint16 toId = ui->spinBox_TX_Request_ShortId->value();     // Step 1: Get the destination
    //    device ID to which the request is sent
    uint16 toId = EP_ID_BROADCAST_;  //       You can also choose to boardcast the request without
                                     //       specifying the target device ID
    if (EP_SUCC_ ==
        eOD.Write_Ep_Request(
            toId,
            EP_CMD_Q_S1_E_)) {  // Step 2: Write the device ID into the data structure
                                          // (i.e. the request itself) pending to be sent
        EP_ID_TYPE_ txToId;
        char* txPkgData;
        int txPkgSize;
        EP_CMD_TYPE_ txCmd = EP_CMD_Q_S1_E_;  // Step 3: Define what type of data we are
                                                        // requesting for. (in this example, we are
                                                        // requesting Roll-Pitch-Yaw readings.
        if (EP_SUCC_ == eP.On_SendPkg(txCmd, &txToId, &txPkgData,
                                      &txPkgSize)) {  // Step 4: create a package out of the data
                                                      // structure (i.e. the payload) to be sent
            serial.writeBytes(txPkgData, txPkgSize);  // Step 5:  Send the package via Serial Port
        }
    }
    char buffer[255];
    serial.readBytes(buffer, 255, 200);
    parse_data(data, buffer, 255);
}

void IMUSensor::ACC_request(IMUData& data, serialib& serial) {
    uint16 toId = EP_ID_BROADCAST_;
    if (EP_SUCC_ ==
        eOD.Write_Ep_Request(toId, EP_CMD_Raw_GYRO_ACC_MAG_)) {
        EP_ID_TYPE_ txToId;
        char* txPkgData;
        int txPkgSize;
        EP_CMD_TYPE_ txCmd = EP_CMD_Raw_GYRO_ACC_MAG_;
        if (EP_SUCC_ == eP.On_SendPkg(txCmd, &txToId, &txPkgData,
                                      &txPkgSize)) {
            serial.writeBytes(txPkgData, txPkgSize);
        }
    }
    char buffer[255];
    serial.readBytes(buffer, 255, 200);
    parse_data(data, buffer, 255);
}

void IMUSensor::read_Data(serialib& serial){
    IMUData data = {};
    
    IMUSensor::MAG_request(data, serial);
    IMUSensor::RPY_request(data, serial);
    IMUSensor::ACC_request(data, serial);
}

void IMUSensor::sensor_loop() {
    IMUData data = {};
    serialib serial;
    char errorOpening = serial.openDevice(SERIAL_PORT, 115200);
    // If connection fails, return the error code otherwise, display a success message
    if (errorOpening != 1) {
        std::cout << errorOpening;
    }
    IMUSensor::generate_data(data);  // initialize with random data so we have something to publish before the first read

    while (m_running) {
        // get data
        // read_data(serial);

        // Generate random data for testing purposes
        data.roll += random_value(5.0f, -5.0f);
        if (data.roll > 360) {          // Roll back if we exceed 360
            data.roll -= 360;
        } else if (data.roll < 0) {     // Roll forward if we go below 0
            data.roll += 360;
        }
        data.pitch += random_value(5.0f, -5.0f);
        if (data.pitch > 180) {
            data.pitch -= 360;
        } else if (data.pitch < -180) {
            data.pitch += 360;
        }
        data.yaw += random_value(5.0f, -5.0f);
        if (data.yaw > 180) {
            data.yaw -= 360;
        } else if (data.yaw < -180) {
            data.yaw += 360;
        }
        data.battery_temp += random_value(2.0f, -2.0f);
        data.power += random_value(0.0f, -0.001f);
        if (data.power < 0) {
            data.power = 0;
        }

        // publish data
        if (m_callback) {
            m_callback(m_topic, &data, sizeof(data));
        }

        // wait for more data
        std::this_thread::sleep_for(m_update_interval);
    }
    serial.closeDevice();
}
