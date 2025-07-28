// THIS HAS NOT BEEN TESTED YET!!

#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define PORT "/dev/ttyACM0" // replace with actual port
#define BAUD_RATE B115200   // based on geiger counter specs

struct SensorData {
    double h2_1;
    double h2_2;
    double ozone;
};

extern struct SensorData hydrogenOzoneData;

int main()
{
    char buffer[255];

    int serialPort = open(PORT, O_RDWR | O_NOCTTY); // read/write and do not take control of terminal
    if (serialPort < 0)
    {
        std::cout << "Failed to open serial port " << serialPort << std::endl;
        return 1;
    }

    // configure port
    termios tty{};
    if (tcgetattr(serialPort, &tty) != 0)
    {
        std::cout << "Failed to get attributes" << std::endl;
        return 1;
    }

    // set baud rate
    cfsetispeed(&tty, BAUD_RATE);
    cfsetospeed(&tty, BAUD_RATE);

    tty.c_cflag &= ~PARENB; // No parity
    tty.c_cflag &= ~CSTOPB; // One stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      // 8 bits per byte
    tty.c_cflag &= ~CRTSCTS; // No flow control
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // No software flow control
    tty.c_oflag &= ~OPOST;                          // Raw output

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0)
    {
        perror("Failed to set attributes");
        return 1;
    }

    // Read loop: read a line of text and parse floats
    std::string line;
    char ch;
    while (true)
    {
        int n = read(serialPort, &ch, 1);
        if (n > 0)
        {
            if (ch == '\n' || ch == '\r')
            {
                if (!line.empty())
                {
                    float h2_1 = 0, h2_2 = 0, ozone = 0;
                    int parsed = sscanf(line.c_str(), "First H2: %f, Second H2: %f, Ozone: %f", &h2_1, &h2_2, &ozone);
                    if (parsed == 3)
                    {
                        data.h2_1 = h2_1;
                        data.h2_2 = h2_2;
                        data.ozone = ozone;
                        printf("Received floats: %f, %f, %f\n", h2_1, h2_2, ozone);
                    }
                    else
                    {
                        fprintf(stderr, "Parse error or unexpected format: %s\n", line.c_str());
                    }
                }
                line.clear();
            }
            else
            {
                if (line.size() < 254)
                    line += ch;
            }
        }
        else if (n < 0)
        {
            perror("Read error");
            break;
        }
    }

    close(serialPort);
    return 0;
}