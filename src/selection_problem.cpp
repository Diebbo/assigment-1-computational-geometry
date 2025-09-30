#include "merge_sort.hpp"
#include "util.hpp"

#include <cassert>
#include <utility>

/* Given two sorted arrays A and B, find the number of elements to take from
 * each array such that the total number of elements is k and the k-th smallest
 * element is the largest possible.
 *
 * Returns a pair (a_count, b_count) where a_count is the number of elements
 * taken from A and b_count is the number of elements taken from B.
 *
 * If k is greater than the total number of elements in both arrays, returns
 * (A.size(), B.size()).
 *
 * If k is less than or equal to 0, returns (0, 0).
 */
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
