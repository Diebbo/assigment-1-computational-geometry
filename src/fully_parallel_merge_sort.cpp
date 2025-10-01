#include "merge_sort.hpp"

#include <cassert>
#include <vector>

void indent(int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
}

void fully_parallel_merge_sort(std::vector<int> &arr, int left, int right,
                               int depth) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

#pragma omp taskgroup
  {
#pragma omp task shared(arr) untied if (right - left >= (1 << 12))
    fully_parallel_merge_sort(arr, left, mid, depth + 1);
#pragma omp task shared(arr) untied if (right - left >= (1 << 12))
    fully_parallel_merge_sort(arr, mid + 1, right, depth + 1);
  }
  // create the two vectors to merge
  std::vector<int> left_vec;
  std::vector<int> right_vec;
  left_vec.assign(arr.begin() + left, arr.begin() + mid + 1);
  right_vec.assign(arr.begin() + mid + 1, arr.begin() + right + 1);

  // assert(std::is_sorted(left_vec.begin(), left_vec.end()) &&
  //        "Left half is not sorted!");
  // assert(std::is_sorted(right_vec.begin(), right_vec.end()) &&
  //        "Right half is not sorted!");

  // merge them into arr
  
  // print left and arr from left to right
  parallel_merge(left_vec, right_vec, arr, left);

  // std::vector<int> temp(left_vec.size() + right_vec.size());
  // parallel_merge(left_vec, right_vec, temp);
  // std::copy(temp.begin(), temp.end(), arr.begin() + left);
  // assert(std::is_sorted(arr.begin() + left, arr.begin() + right + 1) &&
  //        "Array is not sorted after merge!");
}
