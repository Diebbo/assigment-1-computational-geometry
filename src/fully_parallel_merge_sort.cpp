#include "merge_sort.hpp"

#include <cassert>
#include <vector>

const int MERGE_THRESHOLD = 10000;
const int SORT_THRESHOLD = 1000;

// Parallel merge: binary search + task decomposition
void new_parallel_merge(const std::vector<int> &A, int a_start, int a_end,
                        const std::vector<int> &B, int b_start, int b_end,
                        std::vector<int> &C, int c_start) {
  int a_len = a_end - a_start + 1;
  int b_len = b_end - b_start + 1;

  if (a_len < b_len) {
    return new_parallel_merge(B, b_start, b_end, A, a_start, a_end, C, c_start);
  }

  if (a_len == 0)
    return;

  if (a_len + b_len <= MERGE_THRESHOLD) {
    std::merge(A.begin() + a_start, A.begin() + a_end + 1, B.begin() + b_start,
               B.begin() + b_end + 1, C.begin() + c_start);
    return;
  }

  int a_mid = a_start + a_len / 2;
  int b_mid =
      std::lower_bound(B.begin() + b_start, B.begin() + b_end + 1, A[a_mid]) -
      B.begin();
  int c_mid = c_start + (a_mid - a_start) + (b_mid - b_start);

  C[c_mid] = A[a_mid];

#pragma omp task shared(A, B, C)
  new_parallel_merge(A, a_start, a_mid - 1, B, b_start, b_mid - 1, C, c_start);

#pragma omp task shared(A, B, C)
  new_parallel_merge(A, a_mid + 1, a_end, B, b_mid, b_end, C, c_mid + 1);

#pragma omp taskwait
}


// Optimized mergesort with alternating buffers
void new_parallel_mergesort_rec(std::vector<int> &in, std::vector<int> &out,
                                int start, int end, bool write_to_out) {
  if (end - start + 1 <= SORT_THRESHOLD) {
    if (write_to_out) {
      std::copy(in.begin() + start, in.begin() + end + 1, out.begin() + start);
      std::sort(out.begin() + start, out.begin() + end + 1);
    } else {
      std::sort(in.begin() + start, in.begin() + end + 1);
    }
    return;
  }

  int mid = (start + end) / 2;

#pragma omp task shared(in, out)
  new_parallel_mergesort_rec(in, out, start, mid, !write_to_out);

#pragma omp task shared(in, out)
  new_parallel_mergesort_rec(in, out, mid + 1, end, !write_to_out);

#pragma omp taskwait

  const std::vector<int> &src = write_to_out ? in : out;
  std::vector<int> &dst = write_to_out ? out : in;

  new_parallel_merge(src, start, mid, src, mid + 1, end, dst, start);
}



// Public entry point
void new_parallel_mergesort(std::vector<int> &A) {
  std::vector<int> B(A.size());

#pragma omp parallel
  {
#pragma omp single nowait
    new_parallel_mergesort_rec(A, B, 0, A.size() - 1, true);
  }

  // Make sure sorted data ends up in A
  if (!std::is_sorted(A.begin(), A.end())) {
    std::copy(B.begin(), B.end(), A.begin());
  }
}

void fully_parallel_merge_sort(std::vector<int> &arr, int left, int right,
                               int depth) {

  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

#pragma omp taskgroup
  {
#pragma omp task shared(arr) untied if (right - left >= (1 << 9))
    fully_parallel_merge_sort(arr, left, mid, depth + 1);
#pragma omp task shared(arr) untied if (right - left >= (1 << 9))
    fully_parallel_merge_sort(arr, mid + 1, right, depth + 1);
#pragma omp taskyield
  }

  if (right - left < (1 << 10)) {
    // Use sequential merge for small arrays
    sequential_merge(arr, left, mid, right);
    return;
  }

  std::vector<int> left_vec;
  std::vector<int> right_vec;
  left_vec.assign(arr.begin() + left, arr.begin() + mid + 1);
  right_vec.assign(arr.begin() + mid + 1, arr.begin() + right + 1);

  new_parallel_merge(left_vec, 0, left_vec.size() - 1, right_vec, 0, right_vec.size() - 1, arr, left);
}