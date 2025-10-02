#include "util.hpp"

#include <random>
#include <vector>
#include <ctime>
#include <algorithm>

std::vector<int> init_default_vector(int n) {
  std::vector<int> arr(n);
  for (int i = 0; i < n; i++)
    arr[i] = i + 1;
  std::shuffle(arr.begin(), arr.end(),
               std::default_random_engine(time(nullptr)));
  return arr;
}

std::vector<int> init_sorted_vector(int n) {
  std::vector<int> arr(n);
  for (int i = 0; i < n; i++)
    arr[i] = i + 1;
  return arr;
}
