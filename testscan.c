#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "generator.h"

#define NELEMENTS 100
//TODO find good value for UNIT
#define UNIT 50

#ifdef SEQUENTIAL
#define cilk_spawn
#define cilk_sync
#else
//cilkplus libraries
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#endif

//function definition:
void dataparAdd(int i, int j, int a[], int y[]);
void dataparAdd2(int i, int j, int a[], int y[]);

//Implementation

/*
 * input array = a
 * array with sums = sum
 * length = length of array a
 * Version with register from slide 65 ParComp-HCP-Prefix-sums
 * Info: this is inclusive prefix sum algorithm:
 */
void seqscan(int a[], int sum[], int length) {
  assert(length > 0);
  register int sumReg = a[0];
  sum[0] = a[0];
  for(int i = 1; i < length; i++) {
    sumReg += a[i];
    sum[i] = sumReg;
  }
}

/*
 * Cilk Version of data parallel scan computation
 * input array = a, sums are in this array after the algorithm
 * n = length of array a
 * Version with register from slide 65 ParComp-HCP-Prefix-sums
 * Info: this is inclusive prefix sum algorithm:
 */
void cilkscan(int a[], int n) {
  assert(n > 0);
  if (n == 1) return;

  //TODO check what happens if n/2 is odd - should work but check it
  int *y = (int *)malloc((n/2) * sizeof(int));
  
  //data parallel loop:
  //dataparAdd(0, n/2, a, y);
  for(int i = 0; i < n/2; i++) {
    y[i] = a[2*i] + a[2*i+1];
  }

  //all processors must have completed dataparAdd before we can go recursive - here we call it with y instead of a!!
  cilkscan(y, n/2);
 
  a[1] = y[0];
  //another data parallel loop: (here we wann write to a and need the values from y)
  //dataparAdd2(1, n/2, a, y); 
  for(int i = 1; i < n/2; i++) {
    a[2*i] = y[i-1] + a[2*i];
    a[2*i+1] = y[i];
  }

  if((n%2) != 0) { //if n is odd, treat last number
    a[n-1] = y[n/2-1] + a[n-1];
  }
}

//data parallel loop for adding some elements
void dataparAdd(int i, int j, int a[], int y[]) {
  if (j-i <= UNIT) {
    //printf("switching to seq algorithm, i=%i, j=%i...\n", i, j);
    for(int k = i; k < j; k++) {
      y[k] = a[2*k] + a[2*k+1];
    }
  } else {
    cilk_spawn dataparAdd(i, (i+j)/2, a, y);
    cilk_spawn dataparAdd((i+j)/2 + 1, j, a, y);
    cilk_sync;
  }
}

//data parallel loop for adding some elements
void dataparAdd2(int i, int j, int a[], int y[]) {
  if (j-i <= UNIT) {
    //printf("switching to seq algorithm, i=%i, j=%i...\n", i, j);
    for(int k = i; k < j; k++) {
      a[2*k] = y[k-1] + y[2*k];
      a[2*k+1] = y[k];
    }
  } else {
    cilk_spawn dataparAdd2(i, (i+j)/2, a, y);
    cilk_spawn dataparAdd2((i+j)/2 + 1, j, a, y);
    cilk_sync;
  }
}



int main() {
  int *a= generateIntAscendingNumbers(NELEMENTS);
  //int a[] = {1,2,3,4};
  int *sum= (int *)malloc(NELEMENTS * sizeof(int));

  printf("testing inclusive sum with n=%i\n", NELEMENTS);

  seqscan(a, sum, NELEMENTS);

  //verify solution:
  /*
  printf("\narray with sums= ");
  printArray(sum, NELEMENTS);
  printf("\n");
  */
  assert(sum[0] = a[0]);
  for(int i = 0; i < NELEMENTS-1; i++) assert(sum[i+1] == (sum[i] + a[i+1]));
  printf("verification for seq algorithm done.\n");


  printf("now start cilk algorithm with same input array...\n");


  /*
  printf("\narray to sum= ");
  printArray(a, NELEMENTS);
  printf("\n");
  */
  cilkscan(a, NELEMENTS);

  /*
  printf("\narray with sums= ");
  printArray(a, NELEMENTS);
  printf("\n");
  */

  //verify solution:
  for(int i = 0; i < NELEMENTS; i++) {
    /*
    if(a[i] != sum[i]) 
      printf("error found, i=%i, sum[i]=%i while a[i]=%i\n", i, sum[i], a[i]);
      */
    assert(a[i] == sum[i]);
  }
  printf("\nVerification for parallel algorithm done - same result as sequential algorithm!\n");

  return 0;
}
