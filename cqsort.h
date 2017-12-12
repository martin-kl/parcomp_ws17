/* Definition of cilk parallel quicksort.
 *
 * arr[] --> Array to be sorted,
 * n  --> size of array (end index = n-1)
 * unit  --> threshold for switching to sequential algorithmus
 */
void cilkQuickSort(int arr[], int n, int unit);
