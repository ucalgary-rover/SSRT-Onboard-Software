#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define CLIENT_PORT 9090
#define BUFFER_SIZE 1024

struct mqtt_udp_pkt
{
    size_t  total;          // total packet size (8 bytes data type) (8 + 8 + 32 + 8 + 32)
    size_t  topic_len;      // length of topic in bytes
    char    topic[32];      // char array of topic
    size_t  value_len;      // length of value
    char    value[32];      // char array of topic
};

int main() {
    int sock;
    struct sockaddr_in client_addr;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Bind socket to client address
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        perror("Bind failed");
        close(sock);
        return -1;
    }

    // Receive data from the server
    while (true) {
        ssize_t len = recvfrom(sock, buffer, BUFFER_SIZE, 0, nullptr, nullptr);
        if (len > 0) {
            buffer[len] = '\0'; // Null-terminate the received data
            std::cout << "Received from server: " << buffer << std::endl;
        }
    }

    close(sock);
    return 0;
}
