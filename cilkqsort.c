/* extending cilk qsort from Jesper Larsson Traff, TUW, November 2014, November 2016 */
/* added support for parallel partition step */
/* CilkPlus with gcc */

// cilk_for not supported before gcc 5.0

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/time.h>

#include "sqsort.h"

#ifdef SEQUENTIAL
#define cilk_spawn
#define cilk_sync
#else
// cilkplus libraries
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#endif

// a quick hack
typedef unsigned long long usecs;

usecs mytime()
{
  struct timeval now;
  gettimeofday(&now, NULL);
  return (usecs)now.tv_usec + (usecs)now.tv_sec * 1000000L;
}

void quicksort(int a[], int n, int unit)
{
  //if we have just 1 element left return because there is nothing to do
  if (n<2) return;

  //if we are lover than unit start sequential sort
  if(n <= unit) {
      //INFO: printf not working with optimization flag -O3
      //printf("switching to sequential qsort because we are at n=%i which is lower than unit=%i\n", n, unit);
      seqQuickSort(a, 0, n);
      return;
  }

  int i, j;
  j = n;
  int aa;

  // partition
  int pivot = a[0]; // choose an element non-randomly...
  i = 0;
  //j = n;
  for (;;) {
    while (++i < j && a[i] < pivot);
    while (a[--j] > pivot);
    if (i >= j) break;
    //swap a[i] and a[j]
    aa = a[i];
    a[i] = a[j];
    a[j] = aa;
  }
  // swap pivot (a[0] = pivot and a[j]
  aa = a[0];
  a[0] = a[j];
  a[j] = aa;

  //spawn recursive cilk threads
  cilk_spawn quicksort(a,j, unit);
  cilk_spawn quicksort(a+j+1, n-j-1, unit);
  cilk_sync;
}

int main(int argc, char *argv[])
{
  int i, n;
  int *a;
  int s; // sequence type 0, 1, ...

  unsigned seed;

  int unit = 1000;

  usecs start, stop;

  s = 0;
  n = 1;
  seed = 0;
  for (i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n);
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s);
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
    if (argv[i][1]=='u') i++,sscanf(argv[i],"%d",&unit);
  }

  a = (int*)malloc(n*sizeof(int));

  if (s==0) {
    for (i=0; i<n; i++) a[i] = (100-i)%27;
  } else if (s==1) {
    for (i=0; i<n; i++) a[i] = 27; // worst-case input
  } else {
    srand(seed);
    for (i=0; i<n; i++) a[i] = rand()%n;
  }
  
#ifndef SEQUENTIAL
  // check how many workers are available
  printf("Total number of workers: %d\n",__cilkrts_get_nworkers());
#endif

  start = mytime();
  quicksort(a, n, unit);
  stop = mytime();

  // verify
  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  printf("Sorting time %lld\n",stop-start);

  free(a);
  return 0;
}
