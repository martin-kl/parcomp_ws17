//generates different arrays with size = xxxxx (make a header file for this
#include <stdlib.h>
#include <time.h>
#include "generator.h"

int* generateIntSameNumbers(int size) {
    int* result = (int *)malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++) {
		result[i] = 5;
	}
	return result;
}

int* generateIntAscendingNumbers(int size) {
    int* result = (int *)malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++) {
		result[i] = i+1;
	}
	return result;
}

int* generateIntDescendingNumbers(int size) {
    int* result = (int *)malloc(sizeof(int) * size);
    int number = size;
	for(int i = 0; i < size; i++) {
		result[i] = number;
        number--;
	}
	return result;
}

int* generateIntRandomNumbers(int size, int minNumber, int maxNumber) {
    int difference = maxNumber - minNumber;
    int* result = (int *)malloc(sizeof(int) * size);
    srand((unsigned)time(NULL));
	for(int i = 0; i < size; i++) {
            int random = rand();
            random = random % difference;
            random += minNumber;
            result[i] = random;
	}
	return result;
}
