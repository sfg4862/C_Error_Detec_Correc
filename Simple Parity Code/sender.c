#define _WIN32_WINNT 0x0601
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>   

#define PORT 8080
#define SVIP "34.64.127.194" 
#define BUFFER_SIZE 9

#pragma comment(lib, "ws2_32.lib") 

void setParityDansun(char dataword[], char codeword[], int datawordSize) {
    int check = 0;

    for (int i = 0; i < datawordSize; i++) {
        if (dataword[i] == '1') check++;
        codeword[i] = dataword[i];
    }

    codeword[datawordSize] = (check % 2 != 0) ? '1' : '0';
    codeword[datawordSize + 1] = '\0';
}

int main() {
    char dataword[] = "0100101";
    char codeword[9];
    setParityDansun(dataword, codeword, 7);
    // 오류 상황을 위한 테스트 환경시 사용
    //char testcode[] = "01010111";

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error: %d\n", WSAGetLastError());
        return 1;
    }
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Socket Error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SVIP);

    // 오류 상황을 위한 테스트 환경시 사용
    // if (sendto(sock, testcode, strlen(codeword), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
    if (sendto(sock, codeword, strlen(codeword), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Send failed with error code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Dataword sent: %s To %s\n", dataword, SVIP);
    printf("Codeword sent: %s To %s\n", codeword, SVIP);
    // 오류 상황을 위한 테스트 환경시 사용
    //printf("sent : %s To %s\n", testcode, SVIP);
    closesocket(sock);
    WSACleanup();

    return 0;
}