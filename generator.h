#define ARRAY_SIZE 10
#define RANDOM_INT_MIN 0
#define RANDOM_INT_MAX 100

//functions in generator.c:

int* generateIntSameNumbers(int size, int numberToGenerate);
int* generateIntAscendingNumbers(int size);
int* generateIntDescendingNumbers(int size);
int* generateIntPeriodicNumbers(int size);
int* generateIntRandomNumbers(int size, int minNumber, int maxNumber);

double* generateDoubleSameNumbers(int size);
double* generateDoubleAscendingNumbers(int size);
double* generateDoubleDescendingNumbers(int size);
double* generateDoublePeriodicNumbers(int size);
double* generateDoubleRandomNumbers(int size, double minNumber, double maxNumber);


/* Function to print an array, second parameter is length of arrray */
void printArray(int arr[], int n);
