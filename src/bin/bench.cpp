#include "../merge_sort.hpp"
#include "../util.hpp"

#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstdlib>
#include <ctime>
#include <omp.h>

// Benchmark parallel_merge_sort
static void BM_MergeSort(benchmark::State &state) {
  omp_set_max_active_levels(32);
  omp_set_num_threads(state.range(1));

  // Run the benchmark
  for (auto _ : state) {
    // Initialize the vector
    std::vector<int> arr = init_default_vector(state.range(0));
    #pragma omp parallel
    {
      #pragma omp single
      parallel_merge_sort(arr, 0, arr.size() - 1, 0);
    }
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(BM_MergeSort)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// Benchmark parallel_merge_sort, only generating the input once
static void BM_MergeSort_Optimized(benchmark::State &state) {
  omp_set_max_active_levels(32);
  omp_set_num_threads(state.range(1));

  // Initialize the vector
  std::vector<int> arr = init_default_vector(state.range(0));

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> arr2(arr);
    #pragma omp parallel
    {
      #pragma omp single
      parallel_merge_sort(arr2, 0, arr2.size() - 1, 0);
    }
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_MergeSort_Optimized)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// Benchmark std::sort
static void BM_StdSort(benchmark::State &state) {
  // Initialize the vector
  std::vector<int> arr = init_default_vector(state.range(0));

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> arr2(arr);
    std::sort(arr2.begin(), arr2.end());
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_StdSort)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 1}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// Benchmark fully_parallel_merge_sort
static void BM_FullyParallelMergeSort(benchmark::State &state) {
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(state.range(1));

  // Initialize the vector
  std::vector<int> arr = init_default_vector(state.range(0));

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> arr2(arr);
    #pragma omp parallel
    {
      #pragma omp single
      fully_parallel_merge_sort(arr2, 0, arr2.size() - 1, 0);
    }
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_FullyParallelMergeSort)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// Benchmark sequential_merge
static void BM_SequentialMerge(benchmark::State &state) {
  // Initialize the vectors
  std::vector<int> left = init_default_vector(state.range(0) / 2);
  std::vector<int> right = init_default_vector(state.range(0) / 2);
  std::vector<int> arr(state.range(0));
  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());
  arr.insert(arr.end(), left.begin(), left.end());
  arr.insert(arr.end(), right.begin(), right.end());

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> arr2(arr);
    sequential_merge(arr2, 0, left.size() - 1, arr.size() - 1);
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_SequentialMerge)
    ->RangeMultiplier(2)
    ->Range(8, 8 << 18)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// Benchmark parallel_merge
static void BM_ParallelMerge(benchmark::State &state) {
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(state.range(1));

  // Initialize the vectors
  std::vector<int> left = init_default_vector(state.range(0) / 2);
  std::vector<int> right = init_default_vector(state.range(0) / 2);
  std::vector<int> arr(state.range(0));
  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> left2(left);
    std::vector<int> right2(right);
    #pragma omp parallel
    {
      #pragma omp single
      parallel_merge(left2, right2, arr);
    }
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(BM_ParallelMerge)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

typedef std::vector<int> vec;


// Benchmark parallel_merge
static void bm_parallel_merge(benchmark::State &state) {
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(state.range(1));

  // Initialize a single vector already sorted
  vec arr = init_sorted_vector(state.range(0));
  vec left(arr.begin(), arr.begin() + arr.size() / 2);
  vec right(arr.begin() + arr.size() / 2, arr.end());
 

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> left2(left);
    std::vector<int> right2(right);
    #pragma omp parallel
    {
      #pragma omp single
      new_parallel_merge(left2, 0, left2.size()-1, right2, 0, right2.size()-1, arr, 0);
    }
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(bm_parallel_merge)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

static void bm_parallel_merge_worst(benchmark::State &state) {
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(state.range(1));

  // Initialize a single vector already sorted, but this time i swap them
  vec arr = init_sorted_vector(state.range(0));
  vec left(arr.begin(), arr.begin() + arr.size() / 2);
  vec right(arr.begin() + arr.size() / 2, arr.end());
 

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> left2(left);
    std::vector<int> right2(right);
    #pragma omp parallel
    {
      #pragma omp single
      new_parallel_merge(right2, 0, right2.size(), left2, 0, left2.size(), arr, 0);
    }
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(bm_parallel_merge_worst)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();


// Benchmark parallel_merge
static void bm_sequential_merge(benchmark::State &state) {
  // Initialize a single vector already sorted
  vec arr = init_sorted_vector(state.range(0));
 
  // Run the benchmark
  for (auto _ : state) {
    vec arr2(arr);
    sequential_merge(arr, 0, arr.size() / 2 - 1, arr.size() - 1);
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(bm_sequential_merge)
    ->RangeMultiplier(2)
    ->Range(8, 8 << 18)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_MAIN();
