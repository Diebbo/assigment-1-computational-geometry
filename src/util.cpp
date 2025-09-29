#include "util.hpp"

#include <fstream>
#include <random>
#include <vector>

std::vector<int> read_from_file(const char *filename) {
  std::ifstream infile(filename);

  int length;
  infile >> length;

  std::vector<int> a(length);
  for (int i = 0; i < length; i++)
    infile >> a[i];
  return a;
}

std::vector<int> init_default_vector(int n) {
  std::vector<int> arr(n);
  for (int i = 0; i < n; i++)
    arr[i] = i + 1;
  std::shuffle(arr.begin(), arr.end(), std::default_random_engine(42));
  return arr;
}
