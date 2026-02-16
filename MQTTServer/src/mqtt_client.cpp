#include "mqtt_client.hpp"

MQTTClient::MQTTClient(const std::string& server, const std::string& client_id) {
    client_ = std::make_unique<mqtt::async_client>(server, client_id);
}

MQTTClient::~MQTTClient() {
    try {
        if (client_ && client_->is_connected()) {
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

    client_->connect(connection_options)->wait();
}

void MQTTClient::disconnect() {
    if (client_->is_connected()) {
        client_->disconnect()->wait();
    }
}

void MQTTClient::publish(const std::string& topic, const std::string& payload) {
    if (!client_->is_connected()) {
        throw std::runtime_error("Cannot publish: not connected to broker");
    }

    mqtt::message_ptr message = mqtt::make_message(topic, payload);
    message->set_qos(1);
    message->set_retained(false);

    client_->publish(message)->wait();
}

bool MQTTClient::should_shutdown() const {
    return shutdown_requested_.load();
}

void MQTTClient::request_shutdown() {
    shutdown_requested_.store(true);
}
