#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sorts.h"
#include "shared.h"

#ifdef SEQUENTIAL
#define cilk_spawn
#define cilk_sync
#else
// cilkplus libraries
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#endif

#define UNIT (1000)

void quicksort(int a[], int n, int first);
void _partition(int a[], int low, int high, struct partitionResult * result, int helperArray[], int pivotValue);
void writeBack(int helperArray[],int a[], int n, int p1Smaller, int p2Smaller, int isLargerOne);

// --- --- ---- ---- ---- ---- ---- ---
// Implementation
// --- --- ---- ---- ---- ---- ---- ---

//maxThreads is not needed here, is just for omp implementation!
void quicksortC(int a[], int n, int maxThreads ) {
  quicksort(a, n, 1);
}

void quicksort(int a[], int n, int firstTime) {
  printf("starting quicksort with n=%i\n", n);
  //if we have just 1 element left return because there is nothing to do
  if (n < 2) return;

  //if we are lover than unit start sequential sort
  if(n <= UNIT) {
    quicksortS(a, 0, n-1);
    return;
  }

  if(firstTime == 1) {
    int * helperArray = malloc(sizeof(int) * n);
    //first Call - we can use more than one thread / process
    struct partitionResult res1;
    struct partitionResult res2;
    int pivotIndex = randomNumberBetween(0, n-1);
    int pivotValue = a[pivotIndex];
    //switch pivot to first element
    a[pivotIndex] = a[0];
    a[0] = pivotValue;
    //start by 1 cause a[0] contains pivotvalue

    _partition(a, 1, n/2, &res1, helperArray, pivotValue);
    _partition(a, n/2+1, n-1, &res2, helperArray, pivotValue);

    printf("0 after first spawns\n");

    int overallSmaller = res1.smaller + res2.smaller;
    int overallLarger = res1.larger + res2.larger;

 
    //TODO write result back...
    printf("1 after first write back \n");
    cilk_spawn writeBack(helperArray, a, n, res1.smaller, res2.smaller, 0);
    printf("2 after first write back\n");
    cilk_spawn writeBack(helperArray, a, n, res1.smaller, res2.smaller, 1);
    cilk_sync;

    printf("3 after write back\n");
    //pivotIndex should always be 0 here
    a[overallSmaller] = pivotValue;
    free(helperArray);

    printf("4 after write back\n");
    
    //spawn recursive cilk threads
    cilk_spawn quicksort(a, overallSmaller, 0);
    cilk_spawn quicksort(a+overallSmaller+1, overallLarger, 0);
    cilk_sync;

  }else {
    // partition with just one thread
    int pivot = a[0]; // choose an element non-randomly...
    int i = 0;
    int j = n;
    int aa;
    for (;;) {
      while (++i < j && a[i] < pivot);
      while (a[--j] > pivot);
      if (i >= j) break;
      //swap a[i] and a[j]
      aa = a[i];
      a[i] = a[j];
      a[j] = aa;
    }
    // swap pivot

    /*struct partitionResult result;

    partition(a, low+1, high, &result, pivotValue);
    int pi = low+result.smaller;
    aa = a[low]; a[low] = a[pi]; a[pi] = aa;*/

    aa = a[0]; a[0] = a[j]; a[j] = aa;
    //spawn recursive cilk threads
    cilk_spawn quicksortC(a, j, 0);
    cilk_spawn quicksortC(a+j+1, n-j-1, 0);
    cilk_sync;
  }
}

void _partition(int a[], int low, int high, struct partitionResult * result, int helperArray[], int pivotValue) {
  partition(a, low, high, result, pivotValue);
  //memcopy parallel:
  memcpy(helperArray+low, a+low, (sizeof(int) * (high-low+1)));
}

void writeBack(int helperArray[],int a[], int n, int p1Smaller, int p2Smaller, int isLargerOne) {
  if(isLargerOne == 0) {
    //write smaller ones:
    int ai = 0;
    //on i = 0 is pivot
    for(int i = 1; i <= p1Smaller; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
    //start here with 0 ?!? cause in the mid there is no pivot
    for(int i = 0; i < p2Smaller; i++) {
      a[ai] = helperArray[n/2+i+1];
      ai++;
    }
  }else {
    //write larger ones:
    //plus 1 for pivot
    int ai = p1Smaller + p2Smaller + 1;
    for(int i = 0; i < n/2-p1Smaller; i++) {
      a[ai] = helperArray[p1Smaller+i];
      ai++;
    }
    for(int i = 0; i < n/2-1-p2Smaller; i++) {
      a[ai] = helperArray[n/2+p2Smaller+i];
      ai++;
    }
  }
}
