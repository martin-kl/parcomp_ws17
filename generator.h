#define ARRAY_SIZE 10
#define RANDOM_INT_MIN 0
#define RANDOM_INT_MAX 100

//functions in generator.c:

int* generateIntSameNumbers(int size);
int* generateIntAscendingNumbers(int size);
int* generateIntDescendingNumbers(int size);
int* generateIntRandomNumbers(int size, int minNumber, int maxNumber);
