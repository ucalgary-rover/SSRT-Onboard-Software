#pragma once
#include <mqtt/async_client.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <string>
#include <cstdint>
<<<<<<< HEAD

struct Reading {
    float h2_1;     // ppm or whatever units
    float h2_2;
    float ozone;
=======
#include <geiger_counter.cpp>

struct Reading
{
    float h2_1; // ppm or whatever units
    float h2_2;
    float ozone;
    uint16_t geiger;
>>>>>>> 7de2c4f50a8ca4af267d7063a69df944bd197d9a
    uint64_t ts_ms; // timestamp if we need it
};

/** Blocks long enough to take a fresh sample from every sensor. */
Reading readSensors();

<<<<<<< HEAD

class GasSensorBridge {
    int             fd_;
    std::string     topic_;
    mqtt::async_client& cli_;

    int openSerial(const char* dev) {
        int fd = ::open(dev, O_RDONLY | O_NOCTTY);
        if (fd < 0) throw std::runtime_error("Serial open failed");
=======
class GasSensorBridge
{
    int fd_;
    std::string topic_;
    mqtt::async_client &cli_;

    int openSerial(const char *dev)
    {
        int fd = ::open(dev, O_RDONLY | O_NOCTTY);
        if (fd < 0)
            throw std::runtime_error("Serial open failed");
>>>>>>> 7de2c4f50a8ca4af267d7063a69df944bd197d9a

        termios t{};
        tcgetattr(fd, &t);
        cfsetispeed(&t, B115200);
        cfsetospeed(&t, B115200);
        t.c_cflag = CS8 | CLOCAL | CREAD;
        t.c_iflag = t.c_oflag = t.c_lflag = 0;
        tcsetattr(fd, TCSANOW, &t);
        return fd;
    }

public:
<<<<<<< HEAD
    GasSensorBridge(const char* serialDev,
                    std::string topic,
                    mqtt::async_client& cli)
=======
    GasSensorBridge(const char *serialDev,
                    std::string topic,
                    mqtt::async_client &cli)
>>>>>>> 7de2c4f50a8ca4af267d7063a69df944bd197d9a
        : fd_(openSerial(serialDev)),
          topic_(std::move(topic)),
          cli_(cli) {}

<<<<<<< HEAD
    ~GasSensorBridge() { if (fd_ >= 0) ::close(fd_); }

    [[noreturn]] void loop() {
        char buf[256];
        std::string line;

        for (;;) {
            ssize_t n = ::read(fd_, buf, sizeof(buf));
            if (n <= 0) continue;

            for (ssize_t i = 0; i < n; ++i) {
                char c = buf[i];
                if (c == '\n') {
                    if (!line.empty()) {
=======
    ~GasSensorBridge()
    {
        if (fd_ >= 0)
            ::close(fd_);
    }

    [[noreturn]] void loop()
    {
        char buf[256];
        std::string line;

        for (;;)
        {
            ssize_t n = ::read(fd_, buf, sizeof(buf));
            if (n <= 0)
                continue;

            for (ssize_t i = 0; i < n; ++i)
            {
                char c = buf[i];
                if (c == '\n')
                {
                    if (!line.empty())
                    {
>>>>>>> 7de2c4f50a8ca4af267d7063a69df944bd197d9a
                        auto msg = mqtt::make_message(topic_, line);
                        msg->set_qos(0);
                        cli_.publish(msg);
                        line.clear();
                    }
<<<<<<< HEAD
                } else if (c != '\r') {
=======
                }
                else if (c != '\r')
                {
>>>>>>> 7de2c4f50a8ca4af267d7063a69df944bd197d9a
                    line += c;
                }
            }
        }
    }
};
<<<<<<< HEAD


=======
>>>>>>> 7de2c4f50a8ca4af267d7063a69df944bd197d9a
