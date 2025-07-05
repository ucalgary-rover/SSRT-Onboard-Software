#include <windows.h>
#include <iostream>

#define PORT_NAME "\\\\.\\COM3" // Windows requires this format for COM10+; works for COM1-9 too
#define BAUD_RATE CBR_115200

int main()
{
    HANDLE hSerial = CreateFileA(
        PORT_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to open serial port" << std::endl;
        return 1;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        std::cerr << "Failed to get serial params" << std::endl;
        CloseHandle(hSerial);
        return 1;
    }
    dcbSerialParams.BaudRate = BAUD_RATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.fDtrControl = DTR_CONTROL_DISABLE;
    dcbSerialParams.fRtsControl = RTS_CONTROL_DISABLE;
    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        std::cerr << "Failed to set serial params" << std::endl;
        CloseHandle(hSerial);
        return 1;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    // Read loop
    while (true)
    {
        unsigned char byte;
        DWORD bytesRead;
        if (ReadFile(hSerial, &byte, 1, &bytesRead, NULL) && bytesRead > 0)
        {
            printf("Received: 0x%02X\n", byte);
        }
        else if (GetLastError() != ERROR_SUCCESS)
        {
            std::cerr << "Read error" << std::endl;
            break;
        }
    }

    CloseHandle(hSerial);
    return 0;
}