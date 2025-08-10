#include "IMUDataParse.hpp"

// Step 1, TransducerM communication library instantiation:
EasyObjectDictionary eOD;
EasyProfile eP(&eOD);

/**
 * Serial Data Receive - Example
 * @note This function is called when new serial data is available
 */
void parse_data(
	float *array, //< [INPUT] address of the first element in an array 3 elements long. elements 0-3 will be RPY
	char *rxData, ///< [INPUT] Pointer to the RX data array
	int rxSize	  ///< [INPUT] Size of the RX data array
)
{
	Ep_Header header;
	if (EP_SUCC_ == eP.On_RecvPkg(rxData, rxSize, &header))
	{								   // Step 2: Tell the library that new data has arrived.
									   // It does not matter if the new data only contains a fraction
									   // of a complete data package, nor does it matter if the data stream is corrupted
									   // during the transmission. On_RecvPkg() will only return EP_SUCC_
									   // when a complete and correct package has arrived.
									   // Example Reading of the Short ID of the device who sends the data:
		uint32 fromId = header.fromId; // Step 3.1: Now we are able to read the received payload data

		// header.fromId tells us from which TransducerM the data comes.
		switch (header.cmd)
		{ // Step 3.2: header.cmd tells what kind of data is inside the payload.

		case EP_CMD_RPY_:
		{
			Ep_RPY ep_RPY;
			if (EP_SUCC_ == eOD.Read_Ep_RPY(&ep_RPY))
			{ // Another Example reading of the received Roll Pitch and Yaw
				float roll = ep_RPY.roll;
				float pitch = ep_RPY.pitch;
				float yaw = ep_RPY.yaw;

				//					std::cout<<"roll"<<roll<<"\npitch"<<pitch<<"\nyaw"<<yaw;//use data as you wish
				array[0] = roll;
				array[1] = pitch;
				array[2] = yaw;
			}
		}
		break;
		default:
		{
			std::cout << "wrong data";
			break;
		}
		}
	}
	else
	{
		std::cout << "broken data\n";
	}
}

void RPY_request(float *array, serialib serial, char *buffer)
{
	//    uint16 toId = ui->spinBox_TX_Request_ShortId->value();     // Step 1: Get the destination device ID to which the request is sent
	uint16 toId = EP_ID_BROADCAST_; //       You can also choose to boardcast the request without specifying the target device ID
	if (EP_SUCC_ == eOD.Write_Ep_Request(toId, EP_CMD_RPY_))
	{ // Step 2: Write the device ID into the data structure (i.e. the request itself) pending to be sent
		EP_ID_TYPE_ txToId;
		char *txPkgData;
		int txPkgSize;
		EP_CMD_TYPE_ txCmd = EP_CMD_REQUEST_; // Step 3: Define what type of data we are requesting for. (in this example, we are requesting Roll-Pitch-Yaw readings.
		if (EP_SUCC_ == eP.On_SendPkg(txCmd, &txToId, &txPkgData, &txPkgSize))
		{											 // Step 4: create a package out of the data structure (i.e. the payload) to be sent
			serial.writeBytes(txPkgData, txPkgSize); // Step 5:  Send the package via Serial Port
		}
	}

	char header1 = buffer[0];
	serial.readBytes(buffer, 1, 200, 1);
	char header2 = buffer[0];
	serial.readBytes(buffer, 1, 200, 1);
	int length = buffer[0];
	char rxData[length + 5];
	rxData[0] = header1;
	rxData[1] = header2;
	rxData[2] = length;
	for (int i = 0; i < length; i++)
	{
		serial.readBytes(buffer, 1, 200, 1);
		rxData[i + 3] = (char)(unsigned)buffer[0];
	}
	serial.readBytes(buffer, 1, 200, 1);
	rxData[length + 3] = buffer[0];
	serial.readBytes(buffer, 1, 200, 1);
	rxData[length + 4] = buffer[0];

	parse_data(array, rxData, sizeof(rxData));
}

void IMU_RPY(float *array, int port, char *buffer) // *array is a 3 long array, 0 is roll 1 is pitch 2 is yaw, port is which port the IMU is connected to
{
	serialib serial;
	char errorOpening = serial.openDevice(SERIAL_PORT, 115200);
	// If connection fails, return the error code otherwise, display a success message
	if (errorOpening != 1)
	{
		std::cout << errorOpening;
		//		 	return errorOpening;
	}

	//	    printf ("Successful connection to %s\n","\\\\.\\COM4");
	RPY_request(array, serial, buffer);
	serial.closeDevice();
}

int main()
{

	float RPY[3] = {1, 2, 3};
	char buffer[1];

	const std::chrono::milliseconds interval(100); // 100 ms interval = 10 Hz

	while (true)
	{

		auto start = std::chrono::steady_clock::now();

		// Call your function
		IMU_RPY(RPY, 4, buffer);
		std::cout << "roll " << RPY[0] << "\tpitch " << RPY[1] << "\tyaw " << RPY[2] << "\r";
		// Calculate how much time the function took
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

		// Sleep for the remainder of the 100ms period, if any time is left
		if (elapsed < interval)
		{
			std::this_thread::sleep_for(interval - elapsed);
		}
		else
		{
			std::cerr << "Warning: Execution overran the 100ms cycle!\n";
		}
	}
}