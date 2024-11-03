#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <stdio.h>

#define PORT 8080
#define SVIP "34.64.127.194"
#define BUFFER_SIZE 8
#pragma comment(lib, "ws2_32.lib")

void getHamingCode(char codeWord[], char dataword[]) {
    for (int i = 0; i < 4; i++) {
        codeWord[i] = dataword[i];
    }
    codeWord[6] = codeWord[1] ^ codeWord[2] ^ codeWord[3];
    codeWord[5] = codeWord[0] ^ codeWord[1] ^ codeWord[2];
    codeWord[4] = codeWord[2] ^ codeWord[3] ^ codeWord[0];
    codeWord[7] = '\0';
}

int main() {
    char dataWord[5] = "1010";
    char codeWord[BUFFER_SIZE];
    //오류 검출시 사용
    //char testCode[BUFFER_SIZE] = "0010011";

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

    getHamingCode(codeWord, dataWord);
    // 오류 상황을 위한 테스트 환경시 사용
    // if (sendto(sock, testcode, strlen(codeword), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
    if (sendto(sock, (const char*)codeWord, sizeof(codeWord), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Send failed with error code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Dataword sent: %s To %s\n", dataWord, SVIP);
    printf("Codeword sent: %s To %s\n", codeWord, SVIP);
    closesocket(sock);
    WSACleanup();

    return 0;
}