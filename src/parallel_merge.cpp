#include "merge_sort.hpp"

#include <cassert>
#include <vector>


void parallel_merge(std::vector<int> &A, std::vector<int> &B,
                    std::vector<int> &C, int offset) {
  int n = A.size() + B.size();
  // get number of threads
  int num_threads = omp_get_max_threads();

  if (n < num_threads)
    num_threads = n;

  // print num_threads
  // printf("Using %d threads for parallel merge\n", num_threads);

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
    // printf("Thread %d: k = %d, a_count = %d, b_count = %d\n", i, k, a_count, b_count);
    a_indices[i] = a_count;
    b_indices[i] = b_count;
    k_indices[i] = k;
    // assert(a_count + b_count == k &&
    //        "Selection function returned invalid counts!");
  }

  // // print A and B
  // printf("A: ");
  // for (int v : A)
  //   printf("%d ", v);
  // printf("\n");
  // printf("B: ");
  // for (int v : B)
  //   printf("%d ", v);
  // printf("\n");

  a_indices[num_threads] = A.size();
  b_indices[num_threads] = B.size();
  k_indices[num_threads] = n;

  // // print k_indices
  // printf("k_indices: ");
  // for (int i = 0; i <= num_threads; i++) {
  //   printf("%d ", k_indices[i]);
  // }
  // printf("\n");

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

    // if (k_idx != k_end) {
    //   printf("Error in merging: k_idx %d != k_end %d\n", k_idx, k_end);
    //   printf("a_start: %d, a_end: %d, b_start: %d, b_end: %d\n", a_start, a_end,
    //          b_start, b_end);
    //   if (a_start > a_end || b_start > b_end) {
    //     printf("a indices: ");
    //     // print all A
    //     for (int j = 0; j < a_indices.size(); j++)
    //       printf("%d ", a_indices[j]);
    //     printf("\n");
    //     printf("b indices: ");
    //     for (int j = 0; j < b_indices.size(); j++)
    //       printf("%d ", b_indices[j]);
    //     printf("\n");
    //   }
    // }
  }
}

