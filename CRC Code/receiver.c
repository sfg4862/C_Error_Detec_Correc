#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 8
#define ROWS 5
#define DATAWORD_SIZE 4
#define DIVISOR_SIZE 4


unsigned int checkCRC(unsigned int receivedData, unsigned int divisor) {
    for (int i = 3; i >= 0; i--) {
        if ((receivedData & (1 << (i + DIVISOR_SIZE - 1))) != 0) {
            receivedData = receivedData ^ (divisor << i);
        }
    }
    return receivedData;
}

void intToString(unsigned int changeFrom, int bitCount, char target[]) {
    for (int i = bitCount - 1; i >= 0; i--) {
        target[bitCount - 1 - i] = (changeFrom & (1 << i)) ? '1' : '0';
    }
    target[bitCount] = '\0';
}

unsigned int stringToInt(const char* binaryStr) {
    unsigned int result = 0;
    int length = strlen(binaryStr);

    for (int i = 0; i < length; i++) {
        result = result << 1;
        if (binaryStr[i] == '1') {
            result = result | 1;
        }
    }
    return result;
}

int main() {

    char divisor[] = "1011";
    char receivedData[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = { 0 };

    int server_fd;
    struct sockaddr_in address, client_addr;
    int addrlen = sizeof(client_addr);
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

    printf("Received Data: %s\n", receivedData);

    printf("Checking eror via CRC...\n");
    unsigned int binaryReceivedData = stringToInt(receivedData);
    unsigned int binaryDivisor = stringToInt(divisor);
    unsigned int binaryRemainder = checkCRC(binaryReceivedData, binaryDivisor);
    char remainder[DIVISOR_SIZE - 1];
    intToString(binaryRemainder, DIVISOR_SIZE - 1, remainder);

    if (binaryRemainder == 0)printf("No erros Detected !\n");
    else printf("Error Detected !\n");

}