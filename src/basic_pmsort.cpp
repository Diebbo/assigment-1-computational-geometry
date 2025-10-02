#include "merge_sort.hpp"

// Print helper
void printArray(const std::vector<int> &arr) {
  for (int v : arr)
    printf("%d ", v);
  printf("\n");
}

// Merge two sorted halves
void sequential_merge(std::vector<int> &arr, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  std::vector<int> L(n1), R(n2);
  for (int i = 0; i < n1; i++)
    L[i] = arr[left + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[mid + 1 + j];

  int i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (L[i] <= R[j])
      arr[k++] = L[i++];
    else
      arr[k++] = R[j++];
  }
  while (i < n1)
    arr[k++] = L[i++];
  while (j < n2)
    arr[k++] = R[j++];
}

// Parallel merge sort
// Note that right is _inclusive_
void parallel_merge_sort(std::vector<int> &arr, int left, int right,
                         int depth) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

#pragma omp taskgroup
  {
#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
    parallel_merge_sort(arr, left, mid, depth + 1);

#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
    parallel_merge_sort(arr, mid + 1, right, depth + 1);
  }

  sequential_merge(arr, left, mid, right);
}

std::vector<int> read_input(int n, char *inputs[]) {
  std::vector<int> arr(n);
  for (int i = 0; i < n; i++) {
    arr[i] = atoi(inputs[i]);
  }
  return arr;
}
