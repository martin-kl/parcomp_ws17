/* TUW, October 2011 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include "generator.h"
#include "shared.h"
#include "sorts.h"

void quicksort(int * partialArray, int n, MPI_Comm comm, int recursionQuotient);
void assertSorted(int * a, int n) {
  for (int i=0; i<n-1; i++) assert(a[i]<=a[i+1]);
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
    printf("starting with array:\n");
    printArray(a, n);
  }
  int * partialArray = (int*)malloc(sizeof(int)*(n/size));
  MPI_Scatter(a, n/size, MPI_INT, partialArray, n/size, MPI_INT, 0, MPI_COMM_WORLD);
  free(a);

  if(rank == 0) {
    start = MPI_Wtime();
  }

  //start quicksort
  quicksort(partialArray, n/size, MPI_COMM_WORLD, size);
  printf("rank: %i \n", rank);
  printArray(partialArray, n/size);

  if(rank == 0) {
    stop = MPI_Wtime();
    //assertSorted(a, n);
    printf(" > %f\n", stop-start);
  }

  MPI_Finalize();
  return 0;
}

void quicksort(int * partialArray, int n, MPI_Comm comm, int recursionQuotient) {
  if(n < 2) return;
  if(recursionQuotient == 1) {
    //start sequential quicksort
    quicksortS(partialArray, 0, n-1);
    return;
  }

  int rank,size;
  MPI_Comm_size(comm,&size);
  MPI_Comm_rank(comm,&rank);

  int pivotValue, pivotIndex;
  if (rank == 0) {
    //start = MPI_Wtime();
    pivotIndex = randomNumberBetween(0, n-1);
    pivotValue = partialArray[pivotIndex];
  }
  MPI_Bcast(&pivotIndex, 1, MPI_INT, 0, comm);
  MPI_Bcast(&pivotValue, 1, MPI_INT, 0, comm);
  struct partitionResult partitionResult;
  if (rank*(n/size) <= pivotIndex && pivotIndex < (rank+1)*(n/size)) {
    int localPivotIndex = pivotIndex-rank*(n/size);
    partialArray[localPivotIndex] = partialArray[0];
    partialArray[0] = pivotValue;
    partition(partialArray+1, 0, (n/size)-2, &partitionResult, pivotValue);
    partitionResult.smaller++;
  } else {
    partition(partialArray, 0, (n/size)-1, &partitionResult, pivotValue);
  }
  //printf("rank: %i, pivotValue: %i, \n", rank, pivotValue);
  printArray(partialArray, n/size);

  int partialSize;
  int * newPartialArray;
  if (rank%2 == 0) { //collect smaller values, send larger values
    int numSmallerFromOtherProcess;
    MPI_Sendrecv(
        &partitionResult.larger, 1, MPI_INT, rank+1, 1,
        &numSmallerFromOtherProcess, 1, MPI_INT, rank+1, 1,
        comm, MPI_STATUS_IGNORE);

    partialSize = partitionResult.smaller+numSmallerFromOtherProcess;
    newPartialArray = (int*) malloc(partialSize * sizeof(int));
    memcpy(newPartialArray, partialArray, partitionResult.smaller * sizeof(int));

    MPI_Sendrecv(
        partialArray+partitionResult.smaller, partitionResult.larger, MPI_INT, rank+1, 2,
        newPartialArray+partitionResult.smaller, numSmallerFromOtherProcess, MPI_INT, rank+1, 2,
        comm, MPI_STATUS_IGNORE);
  } else { //collect larger values, send smaller values
    int numLargerFromOtherProcess;
    MPI_Sendrecv(
        &partitionResult.smaller, 1, MPI_INT, rank-1, 1,
        &numLargerFromOtherProcess, 1, MPI_INT, rank-1, 1,
        comm, MPI_STATUS_IGNORE);
    partialSize = partitionResult.larger+numLargerFromOtherProcess;
    newPartialArray = (int*) malloc(partialSize * sizeof(int));
    //copy larger values into first positions on newPartialArray
    memcpy(newPartialArray, partialArray+partitionResult.smaller, partitionResult.larger * sizeof(int));
    MPI_Sendrecv(
        partialArray, partitionResult.smaller, MPI_INT, rank-1, 2,
        newPartialArray+partitionResult.larger, numLargerFromOtherProcess, MPI_INT, rank-1, 2,
        comm, MPI_STATUS_IGNORE);
  }
  free(partialArray);
  //printf("after exchanging values, rank: %i, pivotValue: %i, \n", rank, pivotValue);
  printArray(newPartialArray, partialSize);

  MPI_Comm commNew;
  MPI_Comm_split(comm, rank%2, 0, &commNew);
  int newRank, newSize;
  MPI_Comm_size(commNew, &newSize);
  MPI_Comm_rank(commNew, &newRank);

  //recursive calls:
  quicksort(newPartialArray, partialSize, commNew, recursionQuotient/2);
}
