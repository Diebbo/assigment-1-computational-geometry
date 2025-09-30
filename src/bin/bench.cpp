#include "../merge_sort.hpp"
#include "../util.hpp"

#include <benchmark/benchmark.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <omp.h>

static void BM_MergeSortRandom(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  std::vector<int> arr = init_default_vector(state.range(0));

  // Run the benchmark
  // omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_max_active_levels(32);
  omp_set_num_threads(state.range(1));
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
BENCHMARK(BM_MergeSortRandom)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();


// test simple parallel merge sort
static void BM_MergeSort(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  std::vector<int> arr = read_from_file("/tmp/data.txt");
  arr.resize(state.range(0));

  // Run the benchmark
  // omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_max_active_levels(32);
  omp_set_num_threads(state.range(1));
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
BENCHMARK(BM_MergeSort)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// benchmark standard sort
static void BM_StdSortRandom(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  std::vector<int> arr = init_default_vector(state.range(0));

  // Run the benchmark
  for (auto _ : state) {
    std::vector<int> arr2(arr);
    std::sort(arr2.begin(), arr2.end());
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_StdSortRandom)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 2}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// test fully parallel merge sort with 
static void BM_FullyParallelMergeSortRandom(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  std::vector<int> arr = init_default_vector(state.range(0));

  // Run the benchmark
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(state.range(1));
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
BENCHMARK(BM_FullyParallelMergeSortRandom)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

// test fully parallel merge sort with 
static void BM_FullyParallelMergeSort(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  std::vector<int> arr = read_from_file("/tmp/data.txt");
  //trim the vector to the size passed from state
  arr.resize(state.range(0));

  // Run the benchmark
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(state.range(1));
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


static void BM_ReadFromFile(benchmark::State &state) {
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(omp_get_num_threads());
  for (auto _ : state) {
    auto arr = read_from_file("/tmp/data.txt");
    parallel_merge_sort(arr, 0, arr.size() - 1);
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(BM_ReadFromFile)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();


static void BM_Generate(benchmark::State &state) {
  omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_num_threads(omp_get_num_threads());
  for (auto _ : state) {
    auto arr = init_default_vector(100000);
    parallel_merge_sort(arr, 0, arr.size() - 1);
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(BM_Generate)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_MAIN();
