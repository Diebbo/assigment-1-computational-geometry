#include "merge_sort.hpp"

#include <cassert>
#include <vector>

void fully_parallel_merge_sort(std::vector<int> &arr, int left, int right,
                               int depth) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

#pragma omp taskgroup
  {
#pragma omp task shared(arr) untied if (right - left >= (1 << 10))
    fully_parallel_merge_sort(arr, left, mid, depth + 1);
#pragma omp task shared(arr) untied if (right - left >= (1 << 10))
    fully_parallel_merge_sort(arr, mid + 1, right, depth + 1);
  }

  std::vector<int> left_vec;
  std::vector<int> right_vec;
  
  left_vec.assign(arr.begin() + left, arr.begin() + mid + 1);
  right_vec.assign(arr.begin() + mid + 1, arr.begin() + right + 1);

  parallel_merge(left_vec, right_vec, arr, left);

}
