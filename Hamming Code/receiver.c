#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 9
#define ROWS 5

int checkHamming(char receivedData[]) {
    int temp[BUFFER_SIZE];
    for (int i = 0; i < BUFFER_SIZE; i++) {
        temp[i] = receivedData[i] - '0';
    }
    int b3 = temp[0], b2 = temp[1], b1 = temp[2], b0 = temp[3];
    int q2 = temp[4], q1 = temp[5], q0 = temp[6];

    int s0 = b2 ^ b1 ^ b0 ^ q0;
    int s1 = b3 ^ b2 ^ b1 ^ q1;
    int s2 = b1 ^ b0 ^ b3 ^ q2;

    int errorIndex = 0;
    if (s2 == 0 && s1 == 0 && s0 == 0) {
        printf("No Errors Detected!\n");
        return 404;
    }
    else if (s2 == 0 && s1 == 0 && s0 == 1) errorIndex = 6;
    else if (s2 == 0 && s1 == 1 && s0 == 0) errorIndex = 5;
    else if (s2 == 0 && s1 == 1 && s0 == 1) errorIndex = 1;
    else if (s2 == 1 && s1 == 0 && s0 == 0) errorIndex = 4;
    else if (s2 == 1 && s1 == 0 && s0 == 1) errorIndex = 3;
    else if (s2 == 1 && s1 == 1 && s0 == 0) errorIndex = 0;
    else if (s2 == 1 && s1 == 1 && s0 == 1) errorIndex = 2;

    return errorIndex;
}

void correctionError(char receivedData[], int errorIndex) {
    if (receivedData[errorIndex] == '1')receivedData[errorIndex] = '0';
    else receivedData[errorIndex] = '1';
}

int main() {
    int server_fd;
    struct sockaddr_in address, client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[BUFFER_SIZE] = { 0 };
    char receivedData[BUFFER_SIZE];

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

    strcpy(receivedData, buffer);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    printf("Received Raw Data:");
    for (int i = 0; i < BUFFER_SIZE; i++)printf("%c", buffer[i]);
    printf("\n");

    printf("Checking HammingCode...\n");
    int errorIndex = checkHamming(receivedData);
    if (errorIndex == 404)return 1;
    printf("Error detected at index: %d\n", errorIndex);
    printf("Restoring data code...\n");
    correctionError(receivedData, errorIndex);
    printf("Restore Successfully! \nResult: %s\n", receivedData);
}