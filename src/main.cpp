#include "merge_sort.h"

#include <cassert>

int main(int argc, char *argv[]) {
  std::srand(time(NULL));

  // get the array size from command line or default to 10
  int n = (argc > 1) ? atoi(argv[1]) : 10;
  std::vector<int> a(n);
  std::generate(a.begin(), a.end(), std::rand);

  printf("Unsorted array:\n");
  printArray(a);

  parallel_merge_sort(a, 0, a.size() - 1);

  printf("Sorted array:\n");
  printArray(a);

  assert(std::is_sorted(a.begin(), a.end()) && "Array is not sorted!");

  return 0;
}
