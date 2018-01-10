#define ARRAY_SIZE 10
#define RANDOM_INT_MIN 0
#define RANDOM_INT_MAX 100

void generateIntSameNumbers(int *a, int size, int numberToGenerate);
void generateIntAscendingNumbers(int *a, int size);
void generateIntDescendingNumbers(int *a, int size);
void generateIntPeriodicNumbers(int *a, int size);
void generateIntRandomNumbers(int *a, int size, int minNumber, int maxNumber);
void generateIntRandomNumbersWithSeed(int *a, int size, int minNumber, int maxNumberm, unsigned seed);

