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
