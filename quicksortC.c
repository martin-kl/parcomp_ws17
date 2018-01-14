#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sorts.h"
#include "shared.h"

#ifdef SEQUENTIAL
#define cilk_spawn
#define cilk_syn
#else
// cilkplus libraries
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#endif

#define UNIT (1000)

void quicksort(int a[], int n);
void quicksort2(int a[], int n, int * helperArray);
void _partition(int a[], int low, int high, struct partitionResult * result, int helperArray[], int pivotValue);
void writeBack(int helperArray[], int a[], int n, struct partitionResult * res1, struct partitionResult * res2, int isSmallerOne);

int already_set_worker_count = 0;
void setThreads(int threads) {
  if (already_set_worker_count == 1) { return; }
  char str[8];
  sprintf(str, "%d", threads);
  if (0!= __cilkrts_set_param("nworkers", str)) {
    printf("Failed to set worker count. Aborting.\n");
    exit(1);
  }
  already_set_worker_count = 1;
}

// --- --- ---- ---- ---- ---- ---- ---
// Implementation
// --- --- ---- ---- ---- ---- ---- ---

//maxThreads is not needed here, is just for omp implementation!
void quicksortC(int a[], int n, int maxThreads ) {
  setThreads(maxThreads);
  quicksort(a, n);
}

void quicksortC2(int a[], int n, int maxThreads ) {
  setThreads(maxThreads);
  int * helperArray = malloc(sizeof(int) * n);
  quicksort2(a, n, helperArray);
  free(helperArray);
}

void quicksort(int a[], int n)
{
  //if we have just 1 element left return because there is nothing to do
  if (n < 2) return;

  //if we are lower than unit start sequential sort
  if(n <= UNIT) {
    quicksortS(a, 0, n-1);
    return;
  }

  if (n<2) return;

  int pivotIndex = randomNumberBetween(0, n-1);
  int pivotValue = a[pivotIndex];
  //switch pivot to first element
  a[pivotIndex] = a[0];
  a[0] = pivotValue;

  struct partitionResult result;
  partition(a, 1, n-1, &result, pivotValue);
  int pi = result.smaller;
  int aa;
  aa = a[0]; a[0] = a[pi]; a[pi] = aa;

  //spawn recursive cilk threads
  cilk_spawn quicksort(a, pi);

  cilk_spawn quicksort(a+pi+1, n-pi-1);

  cilk_sync;
}


// --- --- ---- ---- ---- ---- ---- ---
// second version
// --- --- ---- ---- ---- ---- ---- ---

void quicksort2(int a[], int n, int * helperArray) {

  //if we have just 1 element left return because there is nothing to do
  if (n < 2) return;

  //if we are lover than unit start sequential sort
  if(n <= UNIT) {
    quicksortS(a, 0, n-1);
    return;
  }

  if(n < 10000) { // partition with just one thread
    int pivotIndex = randomNumberBetween(0, n-1);
    int pivotValue = a[pivotIndex];
    //switch pivot to first element
    a[pivotIndex] = a[0];
    a[0] = pivotValue;

    struct partitionResult result;
    partition(a, 1, n-1, &result, pivotValue);
    int pi = result.smaller;
    int aa;
    aa = a[0]; a[0] = a[pi]; a[pi] = aa;

    //spawn recursive cilk threads
    cilk_spawn quicksort2(a, pi, helperArray);
    cilk_spawn quicksort2(a+pi+1, n-pi-1, helperArray+pi+1);
    cilk_sync;

  } else { // use more than one thread
    struct partitionResult res1;
    struct partitionResult res2;

    int pivotIndex = randomNumberBetween(0, n-1);
    int pivotValue = a[pivotIndex];
    //switch pivot to first element
    a[pivotIndex] = a[0];
    a[0] = pivotValue;

    //start by 1 cause a[0] contains pivotvalue
    cilk_spawn _partition(a, 1, n/2, &res1, helperArray, pivotValue);
    cilk_spawn _partition(a, n/2+1, n-1, &res2, helperArray, pivotValue);
    cilk_sync;

    cilk_spawn writeBack(helperArray, a, n, &res1, &res2, 1);
    cilk_spawn writeBack(helperArray, a, n, &res1, &res2, 0);
    cilk_sync;


    int overallSmaller = res1.smaller + res2.smaller;
    int overallLarger = res1.larger + res2.larger;

    //write pivot on correct position
    a[overallSmaller] = pivotValue;

    cilk_spawn quicksort2(a, overallSmaller, helperArray);
    cilk_spawn quicksort2(a+overallSmaller+1, overallLarger, helperArray+overallSmaller+1);
    cilk_sync;
  }
}

void _partition(int a[], int low, int high, struct partitionResult * result, int helperArray[], int pivotValue) {
  partition(a, low, high, result, pivotValue);
  memcpy(helperArray+low, a+low, (sizeof(int) * (high-low+1)));
}

void writeBack(int helperArray[],int a[], int n, struct partitionResult * res1, struct partitionResult * res2, int isSmallerOne) {
  if(isSmallerOne == 1) {
    //write smaller elements
    int ai = 0;
    //write smaller from 1st thread...
    for (int i = 1; i <= res1->smaller; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
    //write smaller from 2nd thread...
    for(int i = n/2+1; i < n/2+1+res2->smaller; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
  }else {
    //write second part
    int ai = res1->smaller + res2->smaller + 1;
    //write larger from 1st thread...
    for(int i = res1->smaller+1; i <= n/2; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
    //write larger from 2nd thread...
    for(int i = n/2+1+res2->smaller; i < n; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
  }
}
