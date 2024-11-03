#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdio.h>
#define PORT 8080
#define SVIP "34.64.127.194"
#define BUFFER_SIZE 9
#define dataWordRows 4
#pragma comment(lib, "ws2_32.lib")

void setParity(char dataword[][8], char codeword[][9], int dataWordSize, int rows) {
    int check = 0;
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < dataWordSize; i++) {
            if (dataword[j][i] == '1') check++;
            codeword[j][i] = dataword[j][i];

        }
        codeword[j][dataWordSize] = (check % 2 != 0) ? '1' : '0';
        codeword[j][dataWordSize + 1] = '\0';
        check = 0;
    }

}

void setParityColumn(char column[], char target[][9], int rows, int codeWordSize) {
    int check = 0;
    for (int i = 0; i < codeWordSize; i++) {
        for (int j = 0; j < rows; j++) {
            if (target[j][i] == '1') check++;
        }
        if (check % 2 != 0) column[i] = '1';
        else column[i] = '0';
        check = 0;
    }
    column[codeWordSize] = '\0';
}

void setFinalCode(char codeWord[][9], char parityColumns[], char finalCode[][9], int rows, int codeWordSize) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < codeWordSize; j++) {
            finalCode[i][j] = codeWord[i][j];
        }
        finalCode[i][codeWordSize] = '\0';
    }
    for (int i = 0; i < codeWordSize; i++) {
        finalCode[rows][i] = parityColumns[i];
    }
    finalCode[rows][codeWordSize] = '\0';
}

int main() {
    char dataWord[][8] = {
        "1101101",
        "0001010",
        "0111001",
        "1000010"
    };

    //오류 검출시 사용
    /*
    char testCode[][BUFFER_SIZE] = {
        "11000011",
        "00010100",
        "01101010",
        "10000100",
        "00111001"
    };*/

    char codeWord[dataWordRows][BUFFER_SIZE];
    char parityColumn[BUFFER_SIZE];
    char finalCode[dataWordRows + 1][BUFFER_SIZE];

    setParity(dataWord, codeWord, 7, dataWordRows);
    setParityColumn(parityColumn, codeWord, dataWordRows, BUFFER_SIZE - 1);
    setFinalCode(codeWord, parityColumn, finalCode, dataWordRows, BUFFER_SIZE - 1);

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
    if (sendto(sock, (const char*)finalCode, sizeof(finalCode), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        printf("Send failed with error code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("Dataword sent To %s\n", SVIP);
    for (int i = 0; i < dataWordRows; i++) {
        printf("%d 행: ", i + 1);
        printf("%s\n", dataWord[i]);
    }
    printf("TestCode(finalCode) sent To % s\n", SVIP);
    for (int i = 0; i < dataWordRows + 1; i++) {
        printf("%d 행: ", i + 1);
        printf("%s\n", finalCode[i]);
    }
    // 오류 상황을 위한 테스트 환경시 사용
    //printf("sent : %s To %s\n", testcode, SVIP);
    closesocket(sock);
    WSACleanup();

    return 0;
}