#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 9

void checkParityDansun(char data[]) {
    int check = 0;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (data[i] == '1') check++;
    }
    if (check % 2 == 0)printf("Error not Catched!\n");
    else printf("Error occured on data!\n");
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

    printf("Waiting for a connection...\n");

    int valread = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
    if (valread < 0) {
        perror("receive failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    char data[BUFFER_SIZE];
    strcpy(data, buffer);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf(" %s 's Message : %s\n", client_ip, data);
    checkParityDansun(data);
    close(server_fd);

    return 0;
}