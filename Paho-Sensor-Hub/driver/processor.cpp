#include <mqtt/async_client.h>
#include <iostream>

const std::string BROKER   = "tcp://127.0.0.1:1883";
const std::string CLIENTID = "processor";
const std::string SRC      = "sensors/+";

class callback : public mqtt::callback {
    mqtt::async_client& cli_;

public:
    explicit callback(mqtt::async_client& cli) : cli_(cli) {}

    void connected(const std::string&) override {
        cli_.subscribe(SRC, 1)->wait();
        std::cout << "Subscribed to " << SRC << std::endl;
    }

    void message_arrived(mqtt::const_message_ptr msg) override {
        std::string dest = "processed/" + msg->get_topic();
        cli_.publish(dest, msg->get_payload())->wait();
        std::cout << "Republished to " << dest << " payload=" << msg->to_string() << '\n';
    }
};

int main() {
    mqtt::async_client cli(BROKER, CLIENTID);
    callback cb(cli);
    cli.set_callback(cb);

    cli.connect()->wait();
    cli.start_consuming();          // non‑blocking read loop
    std::cout << "Processor running... Ctrl+C to quit\n";
    while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
}
