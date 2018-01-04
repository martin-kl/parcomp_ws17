int randomNumberBetween(int low, int high);
struct partitionResult {
  int smaller;
  int larger;
};
void partition(int a[], int start, int end, struct partitionResult * result, int pivotValue);

double mytime();

/* Function to print an array, second parameter is length of arrray */
void printArray(int arr[], int size);
