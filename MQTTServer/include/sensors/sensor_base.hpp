#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include <atomic>
#include <functional>
#include <string>
#include <thread>

// callback type: void callback(topic, data)
using t_sensor_callback = std::function<void(const std::string&, const std::string&)>;

class SensorBase {
protected:
    std::string m_topic;
    t_sensor_callback m_callback;
    std::atomic<bool> m_running{false};
    std::thread m_sensor_thread;

    // overridden by each subclass
    virtual void sensor_loop() = 0;

public:
    SensorBase(const std::string& topic) : m_topic(topic){};
    virtual ~SensorBase();
    void start(t_sensor_callback callback);
    void stop();

    std::string get_topic() const {
        return m_topic;
    }
};

#endif
