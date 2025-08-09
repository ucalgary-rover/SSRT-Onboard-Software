#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define CLIENT_PORT 9090
#define CLIENT_IP "127.0.0.1"
#define BUFFER_SIZE 1024

struct mqtt_udp_pkt
{
    size_t  total;          // total packet size (8 bytes data type) (8 + 8 + 32 + 8 + 32)
    size_t  topic_len;      // length of topic in bytes
    char    topic[32];      // char array of topic
    size_t  value_len;      // length of value
    char    value[32];      // char array of topic
};

void print_packet(const mqtt_udp_pkt *pkt){
    // Print the initialized structure
    printf("Total: %zu\n", pkt->total);
    printf("Topic Length: %zu\n", pkt->topic_len);
    printf("Topic: %s\n", pkt->topic);
    printf("Value Length: %zu\n", pkt->value_len);
    printf("Value: %s\n", pkt->value);
}

void deserialize_mqtt_udp_pkt(const uint8_t *buffer, struct mqtt_udp_pkt *pkt) {
    size_t offset = 0;

    // Deserialize total
    memcpy(&pkt->total, buffer + offset, sizeof(pkt->total));
    offset += sizeof(pkt->total);

    // Deserialize topic_len
    memcpy(&pkt->topic_len, buffer + offset, sizeof(pkt->topic_len));
    offset += sizeof(pkt->topic_len);

    // Deserialize topic
    memcpy(pkt->topic, buffer + offset, sizeof(pkt->topic));
    offset += sizeof(pkt->topic);

    // Deserialize value_len
    memcpy(&pkt->value_len, buffer + offset, sizeof(pkt->value_len));
    offset += sizeof(pkt->value_len);

    // Deserialize value
    memcpy(pkt->value, buffer + offset, sizeof(pkt->value));
    offset += sizeof(pkt->value);
}

int main() {
    int sock;
    struct sockaddr_in server_addr, client_addr;
    uint8_t buffer[BUFFER_SIZE];

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind socket to server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sock);
        return -1;
    }

    // Set client address info (final client)
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    inet_pton(AF_INET, CLIENT_IP, &client_addr.sin_addr);

    ssize_t len = recvfrom(sock, buffer, 88, 0, nullptr, nullptr);
    mqtt_udp_pkt new_packet;
    deserialize_mqtt_udp_pkt(buffer, &new_packet);
    print_packet(&new_packet);

    // Receive and forward data
    // while (true) {
    //     ssize_t len = recvfrom(sock, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
    //     if (len > 0) {
    //         buffer[len] = '\0'; // Null-terminate the received data
    //         std::cout << "Received: " << buffer << std::endl;

    //         // Forward the data to the final client
    //         sendto(sock, buffer, len, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    //     }
    // }

    close(sock);
    return 0;
}
