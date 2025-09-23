#include "merge_sort.hpp"
#include "util.hpp"

#include <benchmark/benchmark.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <omp.h>

static void BM_MergeSort(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  int len = state.range(0);
  std::vector<int> arr = init_default_vector(len);

  // Run the benchmark
  // omp_set_max_active_levels(omp_get_max_active_levels());
  omp_set_max_active_levels(32);
  omp_set_num_threads(state.range(1));
  for (auto _ : state) {
    std::vector<int> arr2(arr);
    parallel_merge_sort(arr2, 0, len - 1);
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_MergeSort)
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
