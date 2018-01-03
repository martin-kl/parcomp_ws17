//generates different arrays with size = xxxxx (make a header file for this
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "generator.h"

void generateIntSameNumbers(int *a, int size, int numberToGenerate){
	for(int i = 0; i < size; i++) {
		a[i] = numberToGenerate;
	}
}

void generateIntAscendingNumbers(int *a, int size) {
	for(int i = 0; i < size; i++) {
		a[i] = i+1;
	}
}

void generateIntDescendingNumbers(int *a, int size) {
  int number = size;
	for(int i = 0; i < size; i++) {
		a[i] = number;
      number--;
	}
}

void generateIntPeriodicNumbers(int *a, int size) {
	for(int i = 0; i < size; i++) {
		a[i] = (100-i)%27;
	}
}

void generateIntRandomNumbers(int *a, int size, int minNumber, int maxNumber) {
  int difference = maxNumber - minNumber;
  srand((unsigned)time(NULL));
	for(int i = 0; i < size; i++) {
    int random = rand();
    random = random % difference;
    random += minNumber;
    a[i] = random;
	}
}
void generateIntRandomNumbersWithSeed(int *a, int size, int minNumber, int maxNumber, unsigned seed) {
  int difference = maxNumber - minNumber;
  srand(seed);
	for(int i = 0; i < size; i++) {
    int random = rand();
    random = random % difference;
    random += minNumber;
    a[i] = random;
	}
}


void printArray(int arr[], int size) {
  printf("[");
  int i;
  for (i=0; i < size-1; i++)
    printf("%d ", arr[i]);
  printf("%d]\n", arr[size-1]);
}


//all methods again for double - but ATTENTION: they still have a malloc in there!
/*
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

double* generateDoublePeriodicNumbers(int size) {
    double* result = (double *)malloc(sizeof(double) * size);
	for(int i = 0; i < size; i++) {
		result[i] = (100 - i) % 27;
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
*/

