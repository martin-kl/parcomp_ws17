/* TUW, October 2011 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include "generator.h"
#include "shared.h"
#include "sorts.h"

int * quicksort(int * partialArray, int n, MPI_Comm comm, int * newSize);
void assertSorted(int * a, int n) {
  for (int i=0; i<n-1; i++) assert(a[i]<=a[i+1]);
}

void _printArray(int arr[], int size, char text[], int rank, int pivotValue) {
  printf("%s - rank=%i, pivotValue=%i\t\t", text, rank, pivotValue);
  if (size == 0) {
    printf("[]\n");
    return;
  }
  printf("[");
  int i;
  for (i=0; i < size-1; i++)
    printf("%d ", arr[i]);
  printf("%d]\n", arr[size-1]);
}


void generateArray(int *a, int s, int n, unsigned seed) {
  //generate array a:
  if (s==0) {
    generateIntPeriodicNumbers(a, n);
  } else if (s==1) {
    generateIntSameNumbers(a, n, 27);
  }else if (s==2) {
    generateIntAscendingNumbers(a, n);
  }else if (s==3) {
    generateIntDescendingNumbers(a, n);
  } else if (s == 4) {
    if(seed != 0)
      generateIntRandomNumbersWithSeed(a, n, 0, 100, seed);
    else
      generateIntRandomNumbers(a, n, 0, 100);
  }else {
    printf("invalid input for type, only 0-4 is valid\n");
    exit(0);
  }
}

int max(int a, int b) {
  if (a<b) return b;
  return a;
}


int main(int argc, char *argv[])
{

  int rank, size;
  int * a;

  MPI_Init(&argc,&argv);

  //variables for quicksort:
  double start, stop;
  int s = 0;
  int n = 1;
  unsigned seed = 0;

  // get rank and size from communicator
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  //now every processor does this - probably not that bad... (but not really needed)
  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }

  //TODO n/size must divide
  assert((size-1 & size) == 0);
  assert(n%size == 0);

  if(rank == 0) {
    //malloc array - is this working if every processor has a? (defined after main header)
    a = (int*)malloc(n*sizeof(int));
    printf("Executing with: -n %i -s %i -S %i\n", n, s, seed);
    generateArray(a, s, n, seed);
    //printf("starting with array:\n");
    _printArray(a, n, "starting with array:", rank, -1);
    printf("\n");
  }
  int * partialArray = (int*)malloc(sizeof(int)*(n/size));
  MPI_Scatter(a, n/size, MPI_INT, partialArray, n/size, MPI_INT, 0, MPI_COMM_WORLD);

  if(rank == 0) {
    start = MPI_Wtime();
    free(a);
  }

  //start quicksort
  int newSize;
  partialArray = quicksort(partialArray, n/size, MPI_COMM_WORLD, &newSize);

  printf("After Quicksort - rank: %i \n", rank);
  printArray(partialArray, newSize);
  assertSorted(partialArray, newSize);

  if(rank == 0) {
    stop = MPI_Wtime();
    //assertSorted(a, n);
    printf(" > %f\n", stop-start);
  }

  MPI_Finalize();
  return 0;
}

int * quicksort(int * partialArray, int n, MPI_Comm comm, int * newSize) {
  int rank,size;
  MPI_Comm_size(comm,&size);
  MPI_Comm_rank(comm,&rank);

  //if the processor is alone in the communicator it's time to sort sequentially
  if(size == 1) {
    *newSize = n;
    //start sequential quicksort
    quicksortS(partialArray, 0, n-1);
    return partialArray;
  }


  int pivotValue, pivotIndex;
  if (rank == 0) {
    //start = MPI_Wtime();
    pivotIndex = randomNumberBetween(0, n-1);
    pivotValue = partialArray[pivotIndex];
  }
  MPI_Bcast(&pivotIndex, 1, MPI_INT, 0, comm);
  MPI_Bcast(&pivotValue, 1, MPI_INT, 0, comm);

  struct partitionResult partitionResult;
  partition(partialArray, 0, n-1, &partitionResult, pivotValue);

  int partialSize;
  int * tempPartialArray;
  if (rank%2 == 0) { //collect smaller values, send larger values
    int numSmallerFromOtherProcess;
    MPI_Sendrecv(
        &partitionResult.larger, 1, MPI_INT, rank+1, 1,
        &numSmallerFromOtherProcess, 1, MPI_INT, rank+1, 1,
        comm, MPI_STATUS_IGNORE);

    partialSize = partitionResult.smaller + numSmallerFromOtherProcess;

    //copy all larger values from this process into tempPartialArray
    tempPartialArray = (int*) malloc(partitionResult.larger * sizeof(int));
    assert(tempPartialArray != NULL);
    memcpy(tempPartialArray, partialArray+partitionResult.smaller, partitionResult.larger * sizeof(int));

    //reallocate partitialArray to size of all smaller elements from both
    partialArray = (int*) realloc(partialArray, partialSize * sizeof(int));

    MPI_Sendrecv(
        tempPartialArray, partitionResult.larger, MPI_INT, rank+1, 2,
        partialArray + partitionResult.smaller, numSmallerFromOtherProcess, MPI_INT, rank+1, 2,
        comm, MPI_STATUS_IGNORE);
  } else { //collect larger values, send smaller values
    int numLargerFromOtherProcess;
    MPI_Sendrecv(
        &partitionResult.smaller, 1, MPI_INT, rank-1, 1,
        &numLargerFromOtherProcess, 1, MPI_INT, rank-1, 1,
        comm, MPI_STATUS_IGNORE);

    partialSize = partitionResult.larger + numLargerFromOtherProcess;
    //copy all smaller values from this process into tempPartialArray
    tempPartialArray = (int*) malloc(partitionResult.smaller * sizeof(int));
    assert(tempPartialArray != NULL);
    //copy smaller values into first positions on tempPartialArray
    memcpy(tempPartialArray, partialArray, partitionResult.smaller * sizeof(int));

    //reallocate partialArray and set larger elements to first position (we need memmove cause memory regions can overlap)
    memmove(partialArray, partialArray+partitionResult.smaller, partitionResult.larger * sizeof(int));
    partialArray = (int*) realloc(partialArray, partialSize * sizeof(int));

    MPI_Sendrecv(
        tempPartialArray, partitionResult.smaller, MPI_INT, rank-1, 2,
        partialArray + partitionResult.larger, numLargerFromOtherProcess, MPI_INT, rank-1, 2,
        comm, MPI_STATUS_IGNORE);
  }
  free(tempPartialArray);

  _printArray(partialArray, partialSize, "after partition and exchanging", rank, pivotValue);

  MPI_Comm commNew;

  MPI_Comm_split(comm, rank%2, 0, &commNew);
  if(comm != MPI_COMM_WORLD)
    MPI_Comm_free(&comm);

  return quicksort(partialArray, partialSize, commNew, newSize);
}
