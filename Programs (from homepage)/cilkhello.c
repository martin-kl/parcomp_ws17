/* TUW, October 2012, November 2014 (changed to cilkplus) */

#include <stdio.h>
#include <stdlib.h>

// cilkplus libraries
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

void hello(int i)
{
  printf("hello world from task %d\n",i);  // task identify yourself
}

int main()
{
  int i;

  // check how many workers are available
  printf("Total number of workers: %d\n",__cilkrts_get_nworkers());

  // just spawn some tasks
  for (i = 0; i < 100; i++) {
    cilk_spawn hello(i);
  }
  cilk_sync;
  
  return 0;
}
