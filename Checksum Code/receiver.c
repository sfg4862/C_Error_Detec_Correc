#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 10
#define ROWS 5
#define DATAWORD_COUNT 4


int checkChecksum(uint16_t receivedData[], int length, uint16_t receivedChecksum) {
    uint32_t checksum = 0;

    for (int i = 0; i < length; i++) {
        checksum += receivedData[i];
        checksum = (checksum & 0xFFFF) + (checksum >> 16);
    }

    checksum += receivedChecksum;
    checksum = (checksum & 0xFFFF) + (checksum >> 16);
    checksum = ~checksum & 0xFFFF;

    return (checksum == 0);
}

int main() {
    int server_fd;
    struct sockaddr_in address, client_addr;
    int addrlen = sizeof(client_addr);

    char buffer[BUFFER_SIZE] = { 0 };



    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for data...\n");


    int valread = recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
    if (valread < 0) {
        perror("receive failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    uint16_t receivedData[DATAWORD_COUNT];
    memcpy(receivedData, buffer, DATAWORD_COUNT * sizeof(uint16_t));
    uint16_t checksum;
    memcpy(&checksum, buffer + DATAWORD_COUNT * sizeof(uint16_t), sizeof(uint16_t));
    int length = sizeof(receivedData) / sizeof(receivedData[0]);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));


    printf("Received Data Words:\n");
    for (int i = 0; i < DATAWORD_COUNT; i++) {
        printf("0x%04X ", receivedData[i]);
    }
    printf("\nReceived Checksum: 0x%04X\n", checksum);
    printf("Start CheckSum check...\n");
    if (checkChecksum(receivedData, length, checksum))printf("Error not Detected!");
    else printf("Error Detected !\n");
}