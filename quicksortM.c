/* TUW, October 2011 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include "generator.h"
#include "shared.h"

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
  int prev;
  char name[MPI_MAX_PROCESSOR_NAME];
  int nlen;
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

  MPI_Get_processor_name(name,&nlen);

  //now every processor does this - probably not that bad... (but not really needed)
  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }

  if(rank == 0) {
    //malloc array - is this working if every processor has a? (defined after main header)
    a = (int*)malloc(n*sizeof(int));
    printf("Executing with: -n %i -s %i -S %i\n", n, s, seed);
    generateArray(a, s, n, seed);
  }
  //TODO n/size must divide
  assert((size-1 & size) == 0);
  assert(n%size == 0);
  int * partialArray = (int*)malloc(sizeof(int)*(n/size));
  MPI_Scatter(a, n/size, MPI_INT, partialArray, n/size, MPI_INT, 0, MPI_COMM_WORLD);
  free(a);

  int pivotValue, pivotIndex;
  if (rank == 0) {
    start = MPI_Wtime();
    pivotIndex = randomNumberBetween(0, n-1);
    pivotValue = a[pivotIndex];
  }
  MPI_Bcast(&pivotIndex, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&pivotValue, 1, MPI_INT, 0, MPI_COMM_WORLD);
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
  printf("rank: %i, pivotValue: %i, \n", rank, pivotValue);
  printArray(partialArray, n/size);

  if (rank%2 == 0) { //collect smaller values, send larger values
    int numSmallerFromOtherProcess;
    MPI_Sendrecv(
        &partitionResult.larger, 1, MPI_INT, rank+1, MPI_ANY_TAG,
        &numSmallerFromOtherProcess, 1, MPI_INT, rank+1, MPI_ANY_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int newPartialArraySize = max(partitionResult.smaller+partitionResult.larger, partitionResult.smaller+numSmallerFromOtherProcess);
    partialArray = (int*) realloc(partialArray, newPartialArraySize);
    MPI_Sendrecv(
        partialArray+partitionResult.smaller, partitionResult.larger, MPI_INT, rank+1, MPI_ANY_TAG,
        partialArray+partitionResult.smaller, numSmallerFromOtherProcess, MPI_INT, rank+1, MPI_ANY_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else { //collect larger values, send smaller values
    int numLargerFromOtherProcess;
    MPI_Sendrecv(
        &partitionResult.smaller, 1, MPI_INT, rank-1, MPI_ANY_TAG,
        &numLargerFromOtherProcess, 1, MPI_INT, rank-1, MPI_ANY_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int newPartialArraySize = max(partitionResult.larger+partitionResult.smaller, partitionResult.larger+numLargerFromOtherProcess);
    partialArray = (int*) realloc(partialArray, newPartialArraySize);
    MPI_Sendrecv(
        partialArray+partitionResult.larger, partitionResult.smaller, MPI_INT, rank+1, MPI_ANY_TAG,
        partialArray+partitionResult.larger, numLargerFromOtherProcess, MPI_INT, rank+1, MPI_ANY_TAG,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  partitionResult.larger

 //if (rank == 0) {
  //  quicksort(a, n);
  //} else {
  //  int * partialArray = (int*)malloc(sizeof(int)*n);
  //  MPI_Recv(into: partialArray);
  //  partition(partialArray, paritionResult);
  //  mpi-send(partialArray);
  //}



  //quicksort(int * a, int n, int rank, int num_processes) {
  //  helperArray;
  //  for (int i = 1; i <num_processes; i++) {
  //    mpi_send(an: i, part of a);
  //  }
  //  for (int i = 1; i <num_processes; i++) {
  //    mpi_recv(from: i, result: a);
  //  }
  //  mpi_xscan(

  //  int pi = partition(a, n);

  //  quicksort(a,  pi);
  //  quicksort(a+pi+q, n-pi);
  //}


  if(rank == 0) {
    stop = MPI_Wtime();
    //assertSorted(a, n);
    printf(" > %f\n", stop-start);
  }

  MPI_Finalize();
  return 0;
}
