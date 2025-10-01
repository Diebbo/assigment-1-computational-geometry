#include "merge_sort.hpp"

#include <cassert>
#include <vector>


void parallel_merge(std::vector<int> &A, std::vector<int> &B,
                    std::vector<int> &C, int offset) {
  int n = A.size() + B.size();
  // get number of threads
  int num_threads = omp_get_max_threads();

  if (n < 2*num_threads)
    num_threads = n / 2;

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


#pragma omp parallel for
  for (int i = 0; i < num_threads; i++) {
    int a_end = a_indices[i + 1];
    int b_end = b_indices[i + 1];

    int a_idx = a_indices[i];
    int b_idx = b_indices[i];
    int k_idx = k_indices[i] + offset;

    while (a_idx < a_end && b_idx < b_end) {
      if (A[a_idx] <= B[b_idx])
        C[k_idx++] = A[a_idx++];
      else
        C[k_idx++] = B[b_idx++];
    }
    while (a_idx < a_end)
      C[k_idx++] = A[a_idx++];
    while (b_idx < b_end)
      C[k_idx++] = B[b_idx++];

  }
}

