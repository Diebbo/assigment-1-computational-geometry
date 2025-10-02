#include "../merge_sort.hpp"
#include "../util.hpp"
#include "../cli.hpp"

#include <cassert>
#include <cstring>
#include <omp.h>

void handle_parallel_merge_sort(int n);
void handle_selection(int n);
void handle_fully_parallel_merge_sort(int n);
void handle_parallel_merge(int n);

/* Usage:
 * ./main -p n : parallel merge sort
 * ./main -s n : selection problem
 * ./main -m n : parallel merge
 * ./main -f n : fully parallel merge sort
 * ./main -p n -d : enable nested parallelism with 4 threads
 * ./main -p n -n : disable nested parallelism
*/
int main(int argc, char *argv[]) {
  push_flag('m', "merge", true, true, "problem", "Run the parallel merge exercise");
  push_flag('s', "selection", true, true, "problem", "Run the Selection Problem exercise");
  push_flag('p', "parallel-msort", true, true, "problem", "Run the Parallel Merge Sort exercise");
  push_flag('f', "fully-parallel-msort", true, true, "problem", "Run the Fully Partial Merge Sort exercise");
  push_flag(' ', "no-nested", false, false, nullptr, "Disable nested parallelism");
  push_flag(' ', "multithread", false, false, nullptr, "Enable multithreading with 4 cores");

  parse_flags(argc, argv);
  print_parsed_flags();

  flag_result *problem = get_flag_group("problem");
  int n = atoi(problem->value);
  if (strcmp(problem->name_long, "merge") == 0) 
    handle_parallel_merge(n);
  else if (strcmp(problem->name_long, "selection") == 0) 
    handle_selection(n);
  else if (strcmp(problem->name_long, "parallel-msort") == 0) 
    handle_parallel_merge_sort(n);
  else if (strcmp(problem->name_long, "fully-parallel-msort") == 0) 
    handle_fully_parallel_merge_sort(n);

  if (flag_result *f = get_flag_value("multithread"); f != nullptr) 
    omp_set_num_threads(4);
  if (flag_result *f = get_flag_value("nested"); f != nullptr) 
    omp_set_nested(1);
  else
    omp_set_nested(0);
  
  return 0;
}

void handle_parallel_merge_sort(int n) {
  std::vector<int> a(n);

  a = init_default_vector(n);

  printf("Unsorted array:\n");
  printArray(a);
  parallel_merge_sort(a, 0, a.size() - 1);

  printf("Sorted array:\n");
  printArray(a);

  assert(std::is_sorted(a.begin(), a.end()) && "Array is not sorted!");
}

void handle_selection(int n) {
  std::vector<int> a(n);
  a = init_default_vector(n);

  int i = n / 2;
  std::vector<int> left(a.begin(), a.begin() + i);
  std::vector<int> right(a.begin() + i, a.end());

  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());

  printf("Sorted arrays:\n");
  printArray(left);
  printArray(right);

  for (int k = 0; k <= n; ++k) {
    printf("Finding split for k=%d\n", k);
    auto [a_count, b_count] = selection(left, right, k);
    printf("Result: A contributes %d elements, B contributes %d elements\n",
           a_count, b_count);
    assert(k < 0 || a_count + b_count == k || k > n);
  }
}

void handle_fully_parallel_merge_sort(int n) {
  std::vector<int> a(n);
  a = init_default_vector(n);

  fully_parallel_merge_sort(a, 0, a.size() - 1);
  assert(std::is_sorted(a.begin(), a.end()) && "Array is not sorted!");
  printf("Array successfully sorted\n");
}

void handle_parallel_merge(int n) {
  std::vector<int> a(n);
  a = init_default_vector(n);

  // split the array at index i = n / 2
  int i = n / 2;
  std::vector<int> left(a.begin(), a.begin() + i);
  std::vector<int> right(a.begin() + i, a.end());

  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());

  printf("Successfully sorted halves:\n");
  std::vector<int> merged(n);
  parallel_merge(left, right, merged);
  assert(std::is_sorted(merged.begin(), merged.end()) &&
         "Array is not sorted!");
  printf("Array successfully merged\n");
}
