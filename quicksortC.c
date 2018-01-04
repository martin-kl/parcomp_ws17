#include "sorts.h"

#ifdef SEQUENTIAL
#define cilk_spawn
#define cilk_syn
#else
// cilkplus libraries
  #include <cilk/cilk.h>
  #include <cilk/cilk_api.h>
#endif
#define UNIT (2)

void quicksort(int a[], int n);

// --- --- ---- ---- ---- ---- ---- --- // Implementation // --- --- ---- ---- ---- ---- ---- --- //maxThreads is not needed here, is just for omp implementation!
void quicksortC(int a[], int n, int maxThreads ) {
 quicksort(a, n);
}

void quicksort(int a[], int n) {
 //if we have just 1 element left return because there is nothing to do
 if (n < 2) return;
 //if we are lover than unit start sequential sort
 if(n <= UNIT) {
   //INFO: printf not working with optimization flag -O3
   //printf("switching to sequential qsort because we are at n=%i which is lower than unit=%i\n", n, unit);
   quicksortS(a, 0, n-1);
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
 /*struct partitionResult result;
 partition(a, low+1, high, &result, pivotValue);
 int pi = low+result.smaller;
 aa = a[low]; a[low] = a[pi]; a[pi] = aa;*/
 aa = a[0]; a[0] = a[j]; a[j] = aa;
 //spawn recursive cilk threads
 //printf("call quicksort recursive with first element in a=%i, j=%i, unit=%i\n", a[0], j, unit);
 cilk_spawn quicksort(a, j);
 //printf("call quicksort recursive with a=%i, j=%i, unit=%i\n", a[j+1], n-j-1, unit);
 cilk_spawn quicksort(a+j+1, n-j-1);
 cilk_sync;
}
