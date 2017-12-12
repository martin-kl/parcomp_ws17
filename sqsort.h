/* Definition of sequential quicksort.
 * We need this cause we call this algorithm when we are lower than a unit value
 * to sort sequential.
 *
 * arr[] --> Array to be sorted,
 * low  --> Starting index,
 * high  --> Ending index (attention, high = sizeOfArray-1)
 * */
void seqQuickSort(int arr[], int low, int high);

/* Function to print an array, second parameter is length of arrray */
void printArray(int arr[], int n);
