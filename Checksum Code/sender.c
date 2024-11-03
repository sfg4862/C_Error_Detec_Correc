
#define _WIN32_WINNT 0x0601
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>


#define PORT 8080
#define SVIP "34.64.127.194" 
#define BUFFER_SIZE 8
#define DATAWORD_SIZE 4
#define DIVISOR_SIZE 4
#pragma comment(lib, "ws2_32.lib") 

unsigned int getRedundency(unsigned int dataWord, unsigned int divisor) {
    dataWord = dataWord << 3;

    for (int i = 3; i >= 0; i--) {
        if ((dataWord & (1 << (i + DIVISOR_SIZE - 1))) != 0) {
            dataWord = dataWord ^ (divisor << i);
        }
    }
    return dataWord;
}

void getCodeWord(char dataWord[], char remainder[], char codeWord[]) {
    for (int i = 0; i < DATAWORD_SIZE; i++) {
        codeWord[i] = dataWord[i];
    }
    for (int i = DATAWORD_SIZE; i < BUFFER_SIZE - 1; i++) {
        codeWord[i] = remainder[i - DATAWORD_SIZE];
    }
    codeWord[BUFFER_SIZE - 1] = '\0';
}

void intToString(unsigned int num, int bits, char* binaryStr) {
    for (int i = bits - 1; i >= 0; i--) {
        binaryStr[bits - 1 - i] = (num & (1 << i)) ? '1' : '0';
    }
    binaryStr[bits] = '\0';
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
    char dataWord[] = "1010";
    char divisor[] = "1011";

    // 오류 상황을 위한 테스트 환경시 사용
    char testcode[] = "1000011";

    char codeWord[BUFFER_SIZE];
    char remainder[4];
    unsigned int binaryDataWord = stringToInt(dataWord);
    unsigned int binaryDivisor = stringToInt(divisor);
    unsigned int binaryRemainder = getRedundency(binaryDataWord, binaryDivisor);

    intToString(binaryRemainder, DIVISOR_SIZE - 1, remainder);

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


    getCodeWord(dataWord, remainder, codeWord);
    // 오류 상황을 위한 테스트 환경시 사용
    //if (sendto(sock, testcode, strlen(testcode), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
    if (sendto(sock, (const char*)codeWord, sizeof(codeWord), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Send failed with error code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Divisor set : %s\n", divisor);
    printf("Remainder : %s\n", remainder);
    printf("Dataword sent: %s To %s\n", dataWord, SVIP);
    printf("CodeWord sent: %s To %s\n", codeWord, SVIP);
    closesocket(sock);
    WSACleanup();

    return 0;
}