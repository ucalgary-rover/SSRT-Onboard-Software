#include "geiger-counter.hpp"

// THIS FILE IS TO BE USED IF WE CAN'T PUT THE GEIGER ON THE ARDUINO WITH THE OTHER SENSORS
// AKA IF IT'S BEING USED ON THE NANO	


int setup() {
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
    
    return serialPort;
}

unsigned char readGeiger(int serialPort) {
    unsigned char byte;
    int n = read(serialPort, &byte, 1);
    if (n > 0)
    {
        // insert useful code to forward to base computer here
        return byte;
    }
    else if (n < 0)
    {
       perror("Read error");
       return byte;
    }
}


int main()
{
    int serialPort = setup();
    if(serialPort != 0) {
       std::cout<<"Failed to initialize"<<std::endl;
    }

    unsigned char geiger;
    // Read loop
    while (true)
    {
        geiger = readGeiger(serialPort);
        printf("Received: %d\n", geiger);
    }

    close(serialPort);
    return 0;
}

