#include <windows.h>
#include <iostream>

#define PORT_NAME "\\\\.\\COM3" // Windows requires this format for COM10+; works for COM1-9 too
#define BAUD_RATE CBR_9600

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

    // Read loop: read a line of text and parse floats
    const int bufSize = 256;
    char buf[bufSize] = {0};
    DWORD bytesRead = 0;
    std::string line;
    while (true)
    {
        // Read one byte at a time until newline
        char ch;
        DWORD nRead = 0;
        BOOL success = ReadFile(hSerial, &ch, 1, &nRead, NULL);
        if (!success || nRead == 0)
        {
            std::cerr << "Read error" << std::endl;
            Sleep(10); // Wait 10 ms before retrying, matching Arduino's sleep
            continue;
        }
        if (ch == '\n' || ch == '\r')
        {
            if (!line.empty())
            {
                // Parse floats from the line
                float h2_1 = 0, h2_2 = 0, ozone = 0;
                // Use sscanf to extract the three floats from the line
                int n = sscanf(line.c_str(), "First H2: %f, Second H2: %f, Ozone: %f", &h2_1, &h2_2, &ozone);
                if (n == 3)
                {
                    printf("Received floats: %f, %f, %f\n", h2_1, h2_2, ozone);
                }
                else
                {
                    std::cerr << "Parse error or unexpected format: " << line << std::endl;
                }
            }
            line.clear();
        }
        else
        {
            if (line.size() < bufSize - 1)
                line += ch;
        }
    }

    CloseHandle(hSerial);
    std::cout << "Exiting" << std::endl;
    return 0;
}