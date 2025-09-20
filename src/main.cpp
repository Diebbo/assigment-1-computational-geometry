#include "merge_sort.h"

#include <cassert>

void handle_parallel_merge_sort(int n);
void handle_selection(int n);
void handle_fully_parallel_merge_sort(int n);
void handle_parallel_merge(int n);

int main(int argc, char *argv[]) {
  // get the array size from command line or default to 10
  int n = (argc > 2) ? atoi(argv[2]) : 10;
  if (argc > 1) {
    // arg parse
    if (argv[1][1] == 'p') {
      handle_parallel_merge_sort(n);
    } else if (argv[1][1] == 's') {
      handle_selection(n);
    } else if (argv[1][1] == 'm') {
      handle_parallel_merge(n);
    } else if (argv[1][1] == 'f') {
      handle_fully_parallel_merge_sort(n);
    } else {
      fprintf(stderr, "Error: Unknown option %s\n", argv[2]);
      return 1;
    }
  } else {
    fprintf(stderr, "Error: Not enough inputs\n");
    return 1;
  }

  omp_set_nested(1);

  if (argc > 3) {
    if (argv[3][1] == 'd') {
      omp_set_num_threads(4);
      printf("Debug mode: Enabled nested parallelism with 4 threads\n");
    }
    if (argv[3][1] == 'n') {
      omp_set_nested(0);
      printf("Disabled nested parallelism\n");
    }
  }
  return 0;
}

void handle_parallel_merge_sort(int n) {
  std::vector<int> a(n);

  init_default_vector(a);

  printf("Unsorted array:\n");
  printArray(a);
  parallel_merge_sort(a, 0, a.size() - 1);

  printf("Sorted array:\n");
  printArray(a);

  assert(std::is_sorted(a.begin(), a.end()) && "Array is not sorted!");
}

void handle_selection(int n) {
  std::vector<int> a(n);
  init_default_vector(a);

  int i = n / 2;
  std::vector<int> left(a.begin(), a.begin() + i);
  std::vector<int> right(a.begin() + i, a.end());

  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());

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
}

void handle_fully_parallel_merge_sort(int n) {
  std::vector<int> a(n);
  init_default_vector(a);

  fully_parallel_merge_sort(a, 0, a.size() - 1);
  // std::vector<int> merged(n);
  // parallel_merge(left, right, merged);
  assert(std::is_sorted(a.begin(), a.end()) && "Array is not sorted!");
  printf("Array successfully sorted\n");
}

void handle_parallel_merge(int n) {
  std::vector<int> a(n);
  init_default_vector(a);

  // split the array at index i = n / 2
  int i = n / 2;
  std::vector<int> left(a.begin(), a.begin() + i);
  std::vector<int> right(a.begin() + i, a.end());

  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());

  printf("Successfully sorted halves:\n");
  std::vector<int> merged(n);
  parallel_merge(left, right, merged);
  assert(std::is_sorted(merged.begin(), merged.end()) &&
         "Array is not sorted!");
  printf("Array successfully merged\n");
}
