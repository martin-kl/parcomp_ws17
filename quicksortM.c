/* TUW, October 2011 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>
#include "generator.h"
#include "shared.h"
#include "sorts.h"

double _quicksort(int * a, int n, int rank, int size);
int * _rec_quicksort(int * partialArray, int n, MPI_Comm comm, int * newSize);

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
  MPI_Init(&argc,&argv);
  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  //variables for quicksort:
  int s = 0;
  int n = 1;
  int c = 1;
  unsigned seed = 0;


  for (int i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n); //length of array
    if (argv[i][1]=='c') i++,sscanf(argv[i],"%d",&c); //length of array
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s); //type of array
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }


  if (n%size != 0) {
    if (rank == 0) { printf("Number of processors must divide n, given n=%i and processors=%i\n.", n, size); }
    MPI_Finalize();
    return 0;
  }

  if ((size-1 & size) != 0) {
    if (rank == 0) { printf("Number of processors must be a power of 2, given processors=%i.\n", size); }
    MPI_Finalize();
    return 0;
  }

  if (rank == 0) { printf("Executing with: -n %i -s %i -S %i -c %i size: %i\n", n, s, seed, c, size); }

  int * a = (int*)malloc(n*sizeof(int));



  double times[c];
  double mean = 0;
  for (int i = 0; i < c; i++) {
    if (rank == 0) { generateArray(a, s, n, seed); }

    double time = _quicksort(a, n, rank, size);
    if (rank == 0) {
      assertSorted(a, n);
      times[i] = time;
      mean += time;
      printf("  > %f\n", time);
    }
  }
  if (rank == 0) {
    free(a);
    printf("\tmean runtime: %f\n", mean/c);
  }
  MPI_Finalize();
  return 0;
}

double _quicksort(int * a, int n, int rank, int size) {
  double start, stop;



  int * partialArray = (int*)malloc(sizeof(int)*(n/size));
  MPI_Scatter(a, n/size, MPI_INT, partialArray, n/size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  if(rank == 0) {
    start = MPI_Wtime();
  }

  int newSize;
  partialArray = _rec_quicksort(partialArray, n/size, MPI_COMM_WORLD, &newSize);


  MPI_Barrier(MPI_COMM_WORLD);
  if(rank == 0) {
    stop = MPI_Wtime();
  }

  int * elementsPerProcess = NULL;
  int * displacementPerProcess = NULL;
  if (rank == 0) {
    elementsPerProcess = (int*) malloc(size*sizeof(int));
    displacementPerProcess = (int*) malloc(size*sizeof(int));
  }
  MPI_Gather(&newSize, 1, MPI_INT, elementsPerProcess, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    int numValues = 0;
    for (int i = 0; i < size; i++) {
      displacementPerProcess[i] = numValues;
      numValues += elementsPerProcess[i];
    }
  }
  MPI_Gatherv(partialArray, newSize, MPI_INT, a, elementsPerProcess, displacementPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
  free(partialArray);
  if (rank == 0) {
    free(elementsPerProcess);
    free(displacementPerProcess);
  }
  return stop-start;
}

int * _rec_quicksort(int * partialArray, int n, MPI_Comm comm, int * newSize) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int rank_in_communicator_for_group;
  MPI_Comm_rank(comm,&rank_in_communicator_for_group);
  int group_size;
  MPI_Comm_size(comm, &group_size);

  //if the processor is alone in the communicator it's time to sort sequentially
  if(group_size == 1) {
    *newSize = n;
    quicksortS(partialArray, 0, n-1);
    return partialArray;
  }



  int pivotValue, pivotIndex;
  if (rank_in_communicator_for_group == 0) {
    if(n == 0) {
      //assume just some value as pivot
      pivotValue = 25;
    }else {
      pivotIndex = randomNumberBetween(0, n-1);
      pivotValue = partialArray[pivotIndex];
    }
  }
  MPI_Bcast(&pivotIndex, 1, MPI_INT, 0, comm);
  MPI_Bcast(&pivotValue, 1, MPI_INT, 0, comm);

  struct partitionResult partitionResult;
  if(n != 0) {
    partition(partialArray, 0, n-1, &partitionResult, pivotValue);
  } else {
    partitionResult.smaller = 0;
    partitionResult.larger = 0;
  }

  int partialSize;
  int * tempPartialArray;
  if (rank_in_communicator_for_group < (group_size/2)) { //collect smaller values, send larger values
    int numSmallerFromOtherProcess;
    int partner_process = rank+(group_size/2);
    MPI_Sendrecv(
        &partitionResult.larger, 1, MPI_INT, partner_process, 1,
        &numSmallerFromOtherProcess, 1, MPI_INT, partner_process, 1,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    partialSize = partitionResult.smaller + numSmallerFromOtherProcess;

    //copy all larger values from this process into tempPartialArray
    tempPartialArray = (int*) malloc(partitionResult.larger * sizeof(int));
    assert(tempPartialArray != NULL);
    memcpy(tempPartialArray, partialArray+partitionResult.smaller, partitionResult.larger * sizeof(int));

    //reallocate partitialArray to size of all smaller elements from both
    partialArray = (int*) realloc(partialArray, partialSize * sizeof(int));

    MPI_Sendrecv(
        tempPartialArray, partitionResult.larger, MPI_INT, partner_process, 2,
        partialArray + partitionResult.smaller, numSmallerFromOtherProcess, MPI_INT, partner_process, 2,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else { //collect larger values, send smaller values
    int numLargerFromOtherProcess;
    int partner_process = rank-(group_size/2);
    MPI_Sendrecv(
        &partitionResult.smaller, 1, MPI_INT, partner_process, 1,
        &numLargerFromOtherProcess, 1, MPI_INT, partner_process, 1,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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
        tempPartialArray, partitionResult.smaller, MPI_INT, partner_process, 2,
        partialArray + partitionResult.larger, numLargerFromOtherProcess, MPI_INT, partner_process, 2,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  free(tempPartialArray);


  MPI_Comm commNew;

  MPI_Comm_split(comm, rank_in_communicator_for_group<(group_size/2), 0, &commNew);
  if(comm != MPI_COMM_WORLD)
    MPI_Comm_free(&comm);

  return _rec_quicksort(partialArray, partialSize, commNew, newSize);
}
