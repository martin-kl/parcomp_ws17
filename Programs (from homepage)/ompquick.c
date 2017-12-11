/* Jesper Larsson Traff, TUW, November 2016 */
/* OpenMP task Quicksort */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <omp.h>

void quicksort(int a[], int n)
{
  int i, j;
  int aa;

  if (n<2) return;

  //printf("which: %d %d\n",omp_get_thread_num(),omp_get_num_threads());

  // partition
  int pivot = a[0]; // choose an element non-randomly...
  i = 0; j = n;
  for (;;) {
    while (++i<j&&a[i]<pivot); // has one advantage
    while (a[--j]>pivot);
    if (i>=j) break;
    aa = a[i]; a[i] = a[j]; a[j] = aa;
  }
  // swap pivot
  aa = a[0]; a[0] = a[j]; a[j] = aa;

#pragma omp task untied if (n>1000)
  quicksort(a,j);
#pragma omp task untied if (n>1000)
  quicksort(a+j+1,n-j-1);
  //#pragma omp taskwait
}

#define MICRO 1000000.0

int main(int argc, char *argv[])
{
  int i, n;
  int *a;
  int s; // sequence type 0, 1, ...

  int threads;

  unsigned seed;

  double start, stop;

  s = 0;
  n = 1;
  seed = 0;
  threads = 0;
  for (i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n);
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s);
    if (argv[i][1]=='t') i++,sscanf(argv[i],"%d",&threads);
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
  }

  a = (int*)malloc(n*sizeof(int));

  if (threads>0) {
    if (threads>omp_get_max_threads()) threads = omp_get_max_threads();
    omp_set_num_threads(threads);
  }

  printf("NUmber of threads %d, n=%d, seed=%d\n",omp_get_num_threads(),n,seed);

  if (s==0) {
    for (i=0; i<n; i++) a[i] = (100-i)%27;
  } else if (s==1) {
    for (i=0; i<n; i++) a[i] = 27; // worst-case input
  } else {
    srand(seed);
    for (i=0; i<n; i++) a[i] = rand()%n;
  }

  start = omp_get_wtime();
#pragma omp parallel
  {
#pragma omp single nowait
    quicksort(a,n);
#pragma omp taskwait
  }
  stop = omp_get_wtime();

  // verify
  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  printf("Sorting time %.2f\n",(stop-start)*MICRO);

  free(a);
  return 0;
}
