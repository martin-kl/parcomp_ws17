/* Definition of sequential quicksort.
 * We need this cause we call this algorithm when we are lower than a unit value
 * to sort sequential.
 *
 * arr[] --> Array to be sorted,
 * low  --> Starting index,
 * high  --> Ending index (attention, high = sizeOfArray-1)
 * */
void quicksortS(int arr[], int low, int high);


//Definition of openmp implementation
void quicksortO(int arr[], int n, int maxThreads);

//Definition of cilk implementation
void quicksortC(int arr[], int n, int maxThreads);

//Definition of mpi implementation
void quicksortM(int arr[], int n, int maxThreads);

void quicksort2(int arr[], int n);
