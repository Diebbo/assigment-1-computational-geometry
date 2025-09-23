#include "merge_sort.h"

#include <cassert>

int main(int argc, char *argv[]) {
  // get the array size from command line or default to 10
  int n = (argc > 1) ? atoi(argv[1]) : 10;
  std::vector<int> a;
  if (argc > 2) {
    // assert the number of inputs matches n
    if (argc - 2 != n) {
      fprintf(stderr, "Error: Expected %d numbers, but got %d\n", n, argc - 2);
      return 1;
    }

    // read the numbers from command line
    a = read_input(n, &argv[2]);
  } else {
    // Fill & shuffle array
    a = init_default_vector(n);
  }

  printf("Unsorted array:\n");
  printArray(a);

  parallel_merge_sort(a, 0, a.size() - 1);

  printf("Sorted array:\n");
  printArray(a);

  assert(std::is_sorted(a.begin(), a.end()) && "Array is not sorted!");

  return 0;
}
