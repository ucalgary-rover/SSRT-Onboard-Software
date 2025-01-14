#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 88

struct mqtt_udp_pkt
{
    size_t  total;          // total packet size (8 bytes data type) (8 + 8 + 32 + 8 + 32)
    size_t  topic_len;      // length of topic in bytes
    char    topic[32];      // char array of topic
    size_t  value_len;      // length of value
    char    value[32];      // char array of topic
};

void serializePacket(const struct mqtt_udp_pkt *pkt, uint8_t *buffer){
    size_t offset = 0;

    // Serialize total
    memcpy(buffer + offset, &pkt->total, sizeof(pkt->total));
    offset += sizeof(pkt->total);

    // Serialize topic_len
    memcpy(buffer + offset, &pkt->topic_len, sizeof(pkt->topic_len));
    offset += sizeof(pkt->topic_len);

    // Serialize topic
    memcpy(buffer + offset, pkt->topic, sizeof(pkt->topic));
    offset += sizeof(pkt->topic);

    // Serialize value_len
    memcpy(buffer + offset, &pkt->value_len, sizeof(pkt->value_len));
    offset += sizeof(pkt->value_len);

    // Serialize value
    memcpy(buffer + offset, pkt->value, sizeof(pkt->value));
    offset += sizeof(pkt->value);
}

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set server address info
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    const mqtt_udp_pkt initialPacket = {88, 32, "Initial Call", 32, "Hello I hope you are great"};
    uint8_t newBuffer[BUFFER_SIZE];
    serializePacket(&initialPacket, newBuffer);

    sendto(sock, newBuffer, 88, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Open file and read content
    // std::ifstream file("lidar_data.txt");
    // if (!file.is_open()) {
    //     perror("Failed to open file");
    //     close(sock);
    //     return -1;
    // }

    // This seems like something that would be inefficent, 
    // in any case, the data would be sent
    // directly from the driver, not a file

    // while (file.getline(buffer, BUFFER_SIZE)) {
    //     // Send data to the server
    //     sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    //     usleep(500000); // Send every 0.5 seconds for demonstration purposes
    // }

    // file.close();

    close(sock);

    return 0;
}
