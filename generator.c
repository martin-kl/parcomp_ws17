//generates different arrays with size = xxxxx (make a header file for this
#include <stdlib.h>
#include <time.h>
#include "generator.h"

int* generateIntSameNumbers(int size) {
    int* result = (int *)malloc(sizeof(int) * size);
	for(int i = 0; i < size; i++) {
		result[i] = 27;
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



double* generateDoubleSameNumbers(int size) {
    double* result = (double *)malloc(sizeof(double) * size);
	for(int i = 0; i < size; i++) {
		result[i] = 27.0;
	}
	return result;
}

double* generateDoubleAscendingNumbers(int size) {
    double* result = (double *)malloc(sizeof(double) * size);
	for(int i = 0; i < size; i++) {
		result[i] = i+1.1;
	}
	return result;
}

double* generateDoubleDescendingNumbers(int size) {
    double* result = (double *)malloc(sizeof(double) * size);
    double number = size;
	for(int i = 0; i < size; i++) {
		result[i] = number;
        number--;
	}
	return result;
}

double* generateDoubleRandomNumbers(int size, double minNumber, double maxNumber) {
    double difference = maxNumber - minNumber;
    double div = RAND_MAX/difference;
    double* result = (double *)malloc(sizeof(double) * size);
    srand((unsigned)time(NULL));
	for(int i = 0; i < size; i++) {
            int random = rand();
            double randDouble = minNumber + (random / div);
            result[i] = randDouble;
	}
	return result;
}
