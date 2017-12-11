/* Jesper Larsson Traff, TUW, November 2014, November 2016 */
/* CilkPlus with gcc */

// cilk_for not supported before gcc 5.0

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <sys/time.h>

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
  gettimeofday(&now,NULL);
  return (usecs)now.tv_usec+(usecs)now.tv_sec*1000000L;
}

void quicksort(int a[], int n)
{
  int i, j;
  int aa;

  if (n<2) return;

  // partition
  int pivot = a[0]; // choose an element non-randomly...
  i = 0; j = n;
  for (;;) {
    while (++i<j&&a[i]<pivot);
    while (a[--j]>pivot);
    if (i>=j) break;
    aa = a[i]; a[i] = a[j]; a[j] = aa;
  }
  // swap pivot
  aa = a[0]; a[0] = a[j]; a[j] = aa;

  cilk_spawn quicksort(a,j);
  cilk_spawn quicksort(a+j+1,n-j-1);
  cilk_sync;
}

int main(int argc, char *argv[])
{
  int i, n;
  int *a;
  int s; // sequence type 0, 1, ...

  unsigned seed;

  usecs start, stop;

  s = 0;
  n = 1;
  seed = 0;
  for (i=1; i<argc&&argv[i][0]=='-'; i++) {
    if (argv[i][1]=='n') i++,sscanf(argv[i],"%d",&n);
    if (argv[i][1]=='s') i++,sscanf(argv[i],"%d",&s);
    if (argv[i][1]=='S') i++,sscanf(argv[i],"%d",&seed);
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
  quicksort(a,n);
  stop = mytime();

  // verify
  for (i=0; i<n-1; i++) assert(a[i]<=a[i+1]);

  printf("Sorting time %lld\n",stop-start);

  free(a);
  return 0;
}
