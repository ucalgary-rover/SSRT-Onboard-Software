#include "mqtt_client.hpp"

MQTTClient::MQTTClient(const std::string& server, const std::string& m_clientid) {
    m_client = std::make_unique<mqtt::async_client>(server, m_clientid);
}

MQTTClient::~MQTTClient() {
    try {
        if (m_client && m_client->is_connected()) {
            disconnect();
        }
    } catch (...) {
        // suppress exceptions if client is already disconnected
    }
}

void MQTTClient::connect() {
    mqtt::connect_options connection_options;
    connection_options.set_clean_session(true);
    connection_options.set_automatic_reconnect(true);

    m_client->connect(connection_options)->wait();
}

void MQTTClient::disconnect() {
    if (m_client->is_connected()) {
        m_client->disconnect()->wait();
    }
}

void MQTTClient::publish(const std::string& topic, const std::string& payload) {
    if (!m_client->is_connected()) {
        throw std::runtime_error("Cannot publish: not connected to broker");
    }

    mqtt::message_ptr message = mqtt::make_message(topic, payload);
    message->set_qos(1);
    message->set_retained(false);

    m_client->publish(message)->wait();
}

bool MQTTClient::should_shutdown() const {
    return m_shutdown_requested.load();
}

void MQTTClient::request_shutdown() {
    m_shutdown_requested.store(true);
}
