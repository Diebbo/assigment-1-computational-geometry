#include "merge_sort.h"

#include <cassert>
#include <vector>

int main(int argc, char *argv[]) {
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

  printf("Successfully sorted halves:\n");

  std::vector<int> merged(n);
  parallel_merge(left, right, merged);
  printf("Array successfully merged\n");
  
  return 0;
}

void parallel_merge(std::vector<int> &A, std::vector<int> &B,
                    std::vector<int> &C) {
  int n = A.size() + B.size();
  // get number of threads
  int num_threads = omp_get_max_threads();

  if (n < num_threads)
    num_threads = n;

  // print num_threads
  printf("Using %d threads for parallel merge\n", num_threads);

  std::vector<int> a_indices(num_threads + 1);
  std::vector<int> b_indices(num_threads + 1);
  std::vector<int> k_indices(num_threads + 1);
  a_indices[0] = 0;
  b_indices[0] = 0;
  k_indices[0] = 0;

  #pragma omp parallel for
  for (int i = 1; i < num_threads; i++) {
    int k = i * n / num_threads;
    auto [a_count, b_count] = selection(A, B, k);
    a_indices[i] = a_count;
    b_indices[i] = b_count;
    k_indices[i] = k;
  }

  a_indices[num_threads] = A.size();
  b_indices[num_threads] = B.size();
  k_indices[num_threads] = n;

  // print k_indices
  printf("k_indices: ");
  for (int i = 0; i <= num_threads; i++) {
    printf("%d ", k_indices[i]);
  }
  printf("\n");

  #pragma omp parallel for
  for (int i = 0; i < num_threads; i++) {
    int a_start = a_indices[i];
    int a_end = a_indices[i + 1];
    int b_start = b_indices[i];
    int b_end = b_indices[i + 1];
    int k_start = k_indices[i];
    int k_end = k_indices[i + 1];
    
    int a_idx = a_start;
    int b_idx = b_start;
    int k_idx = k_start;

    while (a_idx < a_end && b_idx < b_end) {
      if (A[a_idx] <= B[b_idx]) C[k_idx++] = A[a_idx++];
      else C[k_idx++] = B[b_idx++];
    }
    while (a_idx < a_end)
      C[k_idx++] = A[a_idx++];
    while (b_idx < b_end)
      C[k_idx++] = B[b_idx++];

    assert(k_idx == k_end);
  }
}
