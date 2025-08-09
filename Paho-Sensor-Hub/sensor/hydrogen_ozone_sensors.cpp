#include <mqtt/async_client.h>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
<<<<<<< HEAD
#include <nlohmann/json.hpp> // sudo apt install nlohmann-json3-dev
=======
#include <nlohmann/json.hpp>      // sudo apt install nlohmann-json3-dev
>>>>>>> geiger-counter-dani
#include "sensors.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <chrono>
#include <gps.h>
#include <ctime>
<<<<<<< HEAD
=======

>>>>>>> geiger-counter-dani

using json = nlohmann::json; // easier this way, trust

<<<<<<< HEAD
struct Gnss
{
=======
struct Gnss {
>>>>>>> geiger-counter-dani
    double lat{}, lon{}, alt{};
    uint8_t sats{};
    uint8_t mode{};
    bool valid() const { return mode >= MODE_2D; }
};

<<<<<<< HEAD
Gnss fetchGnss(gps_data_t &g)
{
    if (gps_waiting(&g, 0) && gps_read(&g, NULL, 0) < 0)
    {
        return {};
    }

    Gnss gn{};
    if (g.fix.mode >= MODE_2D)
    {
=======
Gnss fetchGnss(gps_data_t& g){
    if (gps_waiting(&g, 0) && gps_read(&g, NULL, 0) < 0){
        return{};
    }


    Gnss gn{};
    if (g.fix.mode >= MODE_2D) {
>>>>>>> geiger-counter-dani
        gn.lat = g.fix.latitude;
        gn.lon = g.fix.longitude;
        gn.alt = g.fix.altitude;
        gn.sats = g.satellites_used;
        gn.mode = g.fix.mode;
    }

    return gn;
}

Reading readSensors()
{
    static int fd = -1;
    if (fd < 0) // open once
        fd = open("/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_85430363938351607160-if00", O_RDWR | O_NOCTTY);

<<<<<<< HEAD
    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0)
        return {}; // handle read error / no data
    buf[n] = '\0';

    int h1, h2, oz, geiger;
    sscanf(buf, "%d,%d,%d,%d", &h1, &h2, &oz, &geiger);

    Reading r{};
    const float VREF = 5.0f;
    r.h2_1 = h1 * VREF / 1023;
    r.h2_2 = h2 * VREF / 1023;
    r.ozone = oz * VREF / 1023;
    r.geiger = geiger;
    r.ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch())
                  .count();
    return r;
}

constexpr char BROKER[] = "tcp://127.0.0.1:1883"; // keep one for all sensors
constexpr char CLIENTID[] = "sensor_1";           // We should have unique IDs for each sensors
constexpr char TOPIC[] = "sensors/sensor_1";      // Indivivual topics for each of the sensors
constexpr auto INTERVAL = std::chrono::seconds(5);
constexpr int QOS = 1; // Quality of service, use 1 for atleast once

int main()
{
=======
Reading readSensors() {
    static int fd = -1;
    if (fd < 0)                      // open once
        fd = open("/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_85430363938351607160-if00", O_RDWR | O_NOCTTY);

    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n <= 0) return {};          // handle read error / no data
    buf[n] = '\0';

    int h1, h2, oz;
    sscanf(buf, "%d,%d,%d", &h1, &h2, &oz);

    Reading r{};
    const float VREF = 5.0f;
    r.h2_1  = h1 * VREF / 1023;
    r.h2_2  = h2 * VREF / 1023;
    r.ozone = oz * VREF / 1023;
    r.ts_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::system_clock::now().time_since_epoch()).count();
    return r;
}

constexpr char BROKER[]   = "tcp://127.0.0.1:1883";  // keep one for all sensors
constexpr char CLIENTID[] = "sensor_1";            // We should have unique IDs for each sensors
constexpr char TOPIC[]    = "sensors/sensor_1";    // Indivivual topics for each of the sensors
constexpr auto INTERVAL = std::chrono::seconds(5);
constexpr int QOS = 1;  // Quality of service, use 1 for atleast once


int main() {
>>>>>>> geiger-counter-dani
    mqtt::async_client cli(BROKER, CLIENTID);
    cli.connect()->wait();

    // connecting to gpsd
    gps_data_t gps;
<<<<<<< HEAD
    if (gps_open("localhost", "2947", &gps) != 0)
    {
        std::cerr << "GNSS unavailable, continuing with gas sensors only\n";
    }
    else
    {
=======
    if (gps_open("localhost", "2947", &gps) != 0) {
        std::cerr << "GNSS unavailable, continuing with gas sensors only\n";
    } else {
>>>>>>> geiger-counter-dani
        gps_stream(&gps, WATCH_ENABLE | WATCH_JSON, nullptr);
    }
    std::cout << "Connected\n";

<<<<<<< HEAD
    while (true)
    {
        Reading r = readSensors(); // <‑‑  taking one sample

        json payload = {// making a JSON object, each one corresponds to what we need
                        {"h2_1", r.h2_1},
                        {"h2_2", r.h2_2},
                        {"ozone", r.ozone},
                        {"geiger", r.geiger},
                        {"ts", r.ts_ms}};

        Gnss gnf = fetchGnss(gps);
        if (gnf.valid())
        {
=======
    while (true) {
        Reading r = readSensors();     // <‑‑  taking one sample

        json payload = {               // making a JSON object, each one corresponds to what we need
            {"h2_1", r.h2_1},
            {"h2_2", r.h2_2},
            {"ozone", r.ozone},
            {"ts",   r.ts_ms}
        };

        Gnss gnf = fetchGnss(gps);
        if (gnf.valid()) {
>>>>>>> geiger-counter-dani
            payload["lat"] = gnf.lat;
            payload["lon"] = gnf.lon;
            payload["alt"] = gnf.alt;
            payload["sats"] = gnf.sats;
            payload["fix"] = gnf.mode;
        }

        std::string body = payload.dump();

<<<<<<< HEAD
        try
        { // publish it
            cli.publish(TOPIC, body, QOS, /*retained=*/false)->wait();
            std::cout << "Published: " << body << '\n';
        }
        catch (const mqtt::exception &e)
        {
=======
        try {                           // publish it
            cli.publish(TOPIC, body, QOS, /*retained=*/false)->wait();
            std::cout << "Published: " << body << '\n';
        } catch (const mqtt::exception& e) {
>>>>>>> geiger-counter-dani
            std::cerr << "ERROR: publish failed: " << e.what() << '\n';
        }
        std::this_thread::sleep_for(INTERVAL);
    }
}
