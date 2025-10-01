#include "../merge_sort.hpp"
#include "../util.hpp"

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
    ->Ranges({{8, 8 << 18}, {1, 2}})
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

BENCHMARK_MAIN();