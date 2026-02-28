#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <mqtt/async_client.h>

#include <atomic>
#include <memory>
#include <string>

class MQTTClient {
private:
    std::unique_ptr<mqtt::async_client> m_client;
    std::atomic<bool> m_shutdown_requested{false};

public:
    MQTTClient(const std::string& server, const std::string& m_clientid);
    ~MQTTClient();

    void connect();
    void disconnect();
    void publish(const std::string& topic, const std::string& payload);
    bool should_shutdown() const;
    void request_shutdown();
};

#endif
