#include <stdio.h>

int hammingDistance(char code1[], char code2[], int length) {
	int hDistance = 0;
	for (int i = 0; i < length; i++) {
		(code1[i] ^ code2[i]) ? hDistance++ : 0;
	}
	return hDistance;
}
int main() {
	char code1[8] = { 0,1,1,0,1,1,1,1 };
	char code2[8] = { 0,0,1,0,0,0,1,1 };
	char* code1p, * code2p;
	code1p = code1;
	code2p = code2;
	printf("Hamming Distance: %d", hammingDistance(code1p, code2p, 8));
}