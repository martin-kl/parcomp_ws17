# Parallel Computing course, TU WIEN

The files were generated for the project in the course *Parallel Computing* during the winter term 2017 (WS 2017) at TU Wien (Vienna).

## Description
We analyze whether the following hypothesis holds or what sort of speedup is achievable:
> An array of C integers can be sorted in parallel achieving almost linear speedup.

Thus, the parallel algorithms should not suffer from the O(n) time partition bottleneck. The input is given in an array of some C basetype (int or double, …) with “<“ as the comparison function.

We implement a QuickSort in three frameworks: OpenMP, Cilk & MPI to test this hypothesis using various parallelization techniques.

The project documentation that includes the problem statement can be found [here](ParComp_Documentation_Klampfer_Claus_TUWien_WS17.pdf).

---

## Build & Run
### OpenMP & Cilk
```
make main
```
generates the ``main`` executable.

Arguments:
- ``-a``: specify the algorithm to run
    - OpenMP (first version: ``o``, second version: ``O``)
    - Cilk (first version: ``c``, second version: ``C``)
- The same arguments can be used to specify a second algorithm to compare them using ``-A``
- ``-n``: the size of the array
- ``-s``: the structure of the array, 0=periodic numbers, 1=same number, 2=ascending, 3=descending, 4=random numbers (if a seed is passed using ``-S``, this seed will be used)
- ``-S`` (Optional): the seed when using random numbers
- ``-t``: the number of threads to use
- ``-c``: the number of rounds to run, in other words: the algorithm will run as many times and the best, worst and average times will be output

### MPI
```
make mpi
```
generates the ``mpi`` executable.

Arguments:
- ``-n``: the size of the array
- ``-s``: the structure of the array, 0=periodic numbers, 1=same number, 2=ascending, 3=descending, 4=random numbers (if a seed is passed using ``-S``, this seed will be used)
- ``-S`` (Optional): the seed when using random numbers
- ``-c``: the number of rounds to run, in other words: the algorithm will run as many times and the best, worst and average times will be outpu

---

## Authors
@calvinclaus & @martin-kl