#include <mqtt/async_client.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp> // sudo apt install nlohmann-json3-dev
#include "sensors.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <chrono>
#include <ctime>

int readGeiger()
{
    static int fd = -1;
    if (fd < 0)
    { // open once
        fd = open("REPLACE WITH ARDUINO NANO STRING", O_RDWR | O_NOCTTY);
    }

    char buf[8];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0)
    {
        return -1; // read error/no data
    }

    int geiger_reading;
    sscanf(buf, "%d", &geiger_reading);

    return geiger_reading;
}