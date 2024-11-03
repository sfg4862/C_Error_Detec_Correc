#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 9
#define ROWS 5

void check2DParity(char receivedData[][BUFFER_SIZE]) {

    printf("Starting 2D Parity Check.\n");
    int check = 0;
    int errorCount = 0;
    //columns
    for (int i = 0; i < ROWS - 1; i++) {
        for (int j = 0; j < BUFFER_SIZE; j++) if (receivedData[i][j] == '1')check++;
        if (check % 2 != 0) { printf("Error Detected on Raws %d !\n", i + 1); errorCount++; }
        check = 0;
    }
    //rows
    for (int j = 0; j < BUFFER_SIZE - 1; j++) {
        for (int i = 0; i < ROWS; i++) if (receivedData[i][j] == '1')check++;
        if (check % 2 != 0) { printf("Error Detected on Columns %d !\n", j + 1); errorCount++; }
        check = 0;
    }
    if (errorCount != 0)printf("2D Parity check process end. Total Error: %d\n", errorCount);
    else printf("No Errors Detected.\n");
}

int main() {
    int server_fd;
    struct sockaddr_in address, client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE * ROWS] = { 0 };
    char receivedData[ROWS][BUFFER_SIZE];
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

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    printf("Received Raw Data:");
    for (int i = 0; i < BUFFER_SIZE * ROWS; i++)printf("%c", buffer[i]);
    printf("\n");

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < BUFFER_SIZE; j++) {
            receivedData[i][j] = buffer[i * BUFFER_SIZE + j];
        }
        receivedData[i][BUFFER_SIZE] = '\0';
    }
    printf("[Sorted Data]\n");
    for (int i = 0; i < ROWS; i++) {
        printf("[%d] rows: %s\n", i + 1, receivedData[i]);
    }
    check2DParity(receivedData);

    close(server_fd);
    return 0;
}