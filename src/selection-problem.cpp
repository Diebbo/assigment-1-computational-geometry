#include "merge_sort.h"

#include <cassert>
#include <utility>

int not_main(int argc, char *argv[]) {
  // get the array size from command line or default to 10
  int n = (argc > 1) ? atoi(argv[1]) : 10;
  std::vector<int> a(n);
  if (argc > 2) {
    // assert the number of inputs matches n
    if (argc - 2 != n) {
      fprintf(stderr, "Error: Expected %d numbers, but got %d\n", n, argc - 2);
      return 1;
    }

    // read the numbers from command line
    read_input(a, n, &argv[2]);
  } else {
    // Fill & shuffle array
    init_default_vector(a);
  }

  // split the array at index i = n / 2
  int i = n / 2;
  std::vector<int> left(a.begin(), a.begin() + i);
  std::vector<int> right(a.begin() + i, a.end());

  parallel_merge_sort(left, 0, left.size() - 1);
  parallel_merge_sort(right, 0, right.size() - 1);

  printf("Sorted arrays:\n");
  printArray(left);
  printArray(right);

  for (int k = 0; k <= n; ++k) {
    printf("Finding split for k=%d\n", k);
    auto [a_count, b_count] = selection(left, right, k);
    printf("Result: A contributes %d elements, B contributes %d elements\n",
           a_count, b_count);
    assert(k < 0 || a_count + b_count == k || k > n);
  }
  return 0;
}

std::pair<int, int> selection(const std::vector<int> &A,
                              const std::vector<int> &B, int k) {
  int n = A.size();
  int m = B.size();

  if (k >= n + m)
    return {n, m};
  if (k <= 0)
    return {0, 0};

  int a_min = std::max(0, k - m);
  int l = a_min;
  int r = std::min(k, n);

  while (l < r) {
    int a = l + (r - l) / 2;
    int b = k - a;

    if (a > a_min && A[a - 1] > B[b]) {
      r = a - 1;
    } else if (B[b - 1] > A[a]) {
      l = a + 1;
    } else {
      return {a, b};
    }
  }
  return {l, k - l};
}
