#include "merge_sort.h"
#include <benchmark/benchmark.h>
#include <cstdlib>

static void BM_MergeSort(benchmark::State &state) {
  std::srand(std::time(NULL));

  // Initialize the vector
  int len = state.range(0);
  std::vector<int> arr = std::vector<int>(len);
  std::generate(arr.begin(), arr.end(), std::rand);

  // Run the benchmark
  omp_set_num_threads(state.range(1));
  for (auto _ : state) {
    parallel_merge_sort(arr, 0, len - 1);
    benchmark::DoNotOptimize(arr);
  }
}
BENCHMARK(BM_MergeSort)->RangeMultiplier(2)->Ranges({{8, 8 << 18}, {1, 8}})->MeasureProcessCPUTime()->UseRealTime();

BENCHMARK_MAIN();
