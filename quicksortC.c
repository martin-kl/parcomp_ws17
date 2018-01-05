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
void quicksort2(int a[], int n);
void _partition(int a[], int low, int high, struct partitionResult * result, int helperArray[], int pivotValue);
void writeBack(int helperArray[], int a[], int n, struct partitionResult * res1, struct partitionResult * res2, int isSmallerOne);

// --- --- ---- ---- ---- ---- ---- ---
// Implementation
// --- --- ---- ---- ---- ---- ---- ---

//maxThreads is not needed here, is just for omp implementation!
void quicksortC(int a[], int n, int maxThreads ) {
 quicksort(a, n);
 //quicksort2(a, n);
}

void quicksort(int a[], int n)
{
  //if we have just 1 element left return because there is nothing to do
  if (n < 2) return;

  //if we are lover than unit start sequential sort
  if(n <= UNIT) {
    //INFO: printf not working with optimization flag -O3
    //printf("switching to sequential qsort because we are at n=%i which is lower than unit=%i\n", n, unit);
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
  //printf("call quicksort recursive with first element in a=%i, j=%i, unit=%i\n", a[0], j, unit);
  cilk_spawn quicksort(a, pi);

  //printf("call quicksort recursive with a=%i, j=%i, unit=%i\n", a[j+1], n-j-1, unit);
  cilk_spawn quicksort(a+pi+1, n-pi-1);

  cilk_sync;
}


// --- --- ---- ---- ---- ---- ---- ---
// second version
// --- --- ---- ---- ---- ---- ---- ---

void quicksort2(int a[], int n) {
  //printf("\nstarting quicksort with n=%i\n", n);

  //if we have just 1 element left return because there is nothing to do
  if (n < 2) return;

  //if we are lover than unit start sequential sort
  if(n <= UNIT) {
    quicksortS(a, 0, n-1);
    return;
  }

  //if there are less than 100 000 elements, jsut use 1 thread/process
  if(n < 10000) {
    // partition with just one thread
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
    cilk_spawn quicksort2(a, pi);
    cilk_spawn quicksort2(a+pi+1, n-pi-1);
    cilk_sync;

  } else {
    //use more than one thread
    //printf("with more threads...\n");
    int * helperArray = malloc(sizeof(int) * n);
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

    int overallSmaller = res1.smaller + res2.smaller;
    int overallLarger = res1.larger + res2.larger;

    //TODO write result back...
    cilk_spawn writeBack(helperArray, a, n, &res1, &res2, 1);
    cilk_spawn writeBack(helperArray, a, n, &res1, &res2, 0);
    cilk_sync;

    /*
    //
    //for testing, write result back just from 1 thread:
    //
    int ai = 0;
    //write smaller from 1st array:
    //start with 1 because on 0 is the pivot and thread 1 just partitioned elements from 1 to n/2 (incl)
    for(int i = 1; i <= res1.smaller; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
    //write smaller from 2st array:, start to read from helper array AFTER first half
    for(int i = n/2+1; i < n/2+1+res2.smaller; i++) {
      a[ai] = helperArray[i];
      ai++;
    }

    //increment ai to let one element for overallSmaller:
    //this assert was just for testing and is true
    //assert(ai == overallSmaller); //ai must be the index for the pivot here
    //we can even write the pivot here... but we do it in the end
    //a[ai] = pivotValue;
    //increment ai to start after pivot....
    ai++;

    //write back larger from 1st array, start to write after pivot:
    //+1 needed here cause pivot is again on pos 0
    for(int i = res1.smaller+1; i <= n/2; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
    //write back larger from 1st array, start to write after pivot:
    for(int i = n/2+1+res2.smaller; i < n; i++) {
      a[ai] = helperArray[i];
      ai++;
    }
    */

    //write pivot on correct position
    a[res1.smaller+res2.smaller] = pivotValue;
    //a[overallSmaller] = pivotValue;
    free(helperArray);

    //printf("result:\n");
    //printArray(a,n);

    //spawn recursive cilk threads
    cilk_spawn quicksort2(a, overallSmaller);
    cilk_spawn quicksort2(a+overallSmaller+1, overallLarger);
    cilk_sync;
  }
}

void _partition(int a[], int low, int high, struct partitionResult * result, int helperArray[], int pivotValue) {
  partition(a, low, high, result, pivotValue);
  //memcopy parallel:
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
/*
      //old version:
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
  */
}
