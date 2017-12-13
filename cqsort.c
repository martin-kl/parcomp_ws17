#include "cqsort.h"
#include "sqsort.h"

#ifdef SEQUENTIAL
#define cilk_spawn
#define cilk_sync
#else
// cilkplus libraries
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#endif


void cilkQuickSort(int a[], int n, int unit)
{
  //if we have just 1 element left return because there is nothing to do
  if (n < 2) return;

  //if we are lover than unit start sequential sort
  if(n <= unit) {
      //INFO: printf not working with optimization flag -O3
      //printf("switching to sequential qsort because we are at n=%i which is lower than unit=%i\n", n, unit);
      seqQuickSort(a, 0, n-1);
      return;
  }

  int i, j;
  int aa;

  if (n<2) return;

  // partition
  int pivot = a[0]; // choose an element non-randomly...
  i = 0; j = n;
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
  aa = a[0]; a[0] = a[j]; a[j] = aa;

  //spawn recursive cilk threads
  //printf("call quicksort recursive with first element in a=%i, j=%i, unit=%i\n", a[0], j, unit);
  cilk_spawn cilkQuickSort(a, j, unit);
  
  //printf("call quicksort recursive with a=%i, j=%i, unit=%i\n", a[j+1], n-j-1, unit);
  cilk_spawn cilkQuickSort(a+j+1, n-j-1, unit);

  cilk_sync;
}
