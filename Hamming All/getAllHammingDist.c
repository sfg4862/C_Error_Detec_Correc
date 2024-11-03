#include <stdio.h>

int getHammingDistance(char target[], char code[], int length) {
    int hammingDistance = 0;
    for (int i = 0; i < length; i++) (target[i] ^ code[i]) ? hammingDistance++ : 0;
    return hammingDistance;
}
int main() {
    char* bits[32] = {
        "00000", "00001", "00010", "00011", "00100", "00101",
        "00110", "00111", "01000", "01001", "01010", "01011",
        "01100", "01101", "01110", "01111", "10000", "10001",
        "10010", "10011", "10100", "10101", "10110", "10111",
        "11000", "11001", "11010", "11011", "11100", "11101",
        "11110", "11111“
    };
    int pass = 0;
    printf("비트 00000에 대한 모든 해밍거리 경우의 수\n");
    for (int i = 1; i < 32; i++) {
        if (pass == 2) { printf("\n"); pass = 0; }
        printf("%s 과 %s 의 해밍거리 dmin = %d    ", bits[0], bits[i], getHammingDistance(bits[0], bits[i], 5));
        pass++;
    }

}