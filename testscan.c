#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "generator.h"

#define NELEMENTS 1000

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


int main() {
  int *a= generateIntSameNumbers(NELEMENTS, 1);
  int *sum= (int *)malloc(NELEMENTS * sizeof(int));

  printf("testing inclusive sum with n=%i and array with all numbers set to 1\n", NELEMENTS);

  seqscan(a, sum, NELEMENTS);

  //verify solution:
  /*
  printf("\narray with sums= ");
  printArray(sum, NELEMENTS);
  printf("\n\n");
  */
  assert(sum[0] = 1);
  for(int i = 0; i < NELEMENTS-1; i++) assert(sum[i] == (sum[i+1] - 1));
  printf("verification done.\n");

  return 0;
}
