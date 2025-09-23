#import "@preview/problemst:0.1.2": pset

#show: pset.with(
  class: "Computational geometry",
  student: "",
  title: "Parallel Merge Sort",
  date: datetime.today(),
  collaborators: ("Diego Barbieri", "Samuele Esposito", "Gioele Scandaletti"),
)

= Basic Parallel Merge Sort

Firstly, we need to define the tech stack we are going to use for this project. We choose to use *C++* with the library *OpenMP* for parallelization. The reason behind this choice is oc the high performance and the low level control that the language offers.

== Implementation

We started from the classic implementation of the Merge Sort algorithm, which is a divide and conquer algorithm that works by recursively dividing the array into two halves, sorting each half, and then merging the two sorted halves back together. The following code already includes the `#pragma` directives needed for the parallelization:

```cpp
void parallel_merge_sort(std::vector<int> &arr, int left, int right,
                         int depth) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

#pragma omp taskgroup
  {
#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
    parallel_merge_sort(arr, left, mid, depth + 1);

#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
    parallel_merge_sort(arr, mid + 1, right, depth + 1);
  }

  merge(arr, left, mid, right);
}
```

== Performance

In order to measure the performance of out implementation we used the tool *perf* on a Linux machine.
It's important to note that the performance in this case are highly dependent on the hardware and the specific implementation details. Moreover, we will run the comparison on the same machine to ensure a fair comparison, with the following specifications:
- CPU: Intel i5-1135G7, 4 cores, 8 threads, 4.2 GHz
- RAM: 8 GB
- OS: Arch Linux - Kernel 6.16.7-arch1-1

The results of our first implementation are the following:

#figure(
  image("./comp-4-cpus.png", width: 90%),
  caption: "Comparison of the performance of the parallel merge sort with different number of threads"
)<fig:comp-4-cpus>

As a first test, we tried comparing the performance of our implementation on the same machine with different number of threads involved(see @fig:comp-4-cpus). The results maintained our expectations: as we increase the number of threads, the performance improves. However, we can see that the improvement is not linear, and after a certain point, the performance starts to degrade.

One of the hypotheses behind this behavior is that there could be an overhead caused by the generation of the array.

```cpp
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
    #pragma omp parallel
    {
      #pragma omp single
      parallel_merge_sort(arr, 0, arr.size() - 1, 0);
    }
    benchmark::DoNotOptimize(arr2);
  }
}
BENCHMARK(BM_MergeSort)
    ->RangeMultiplier(2)
    ->Ranges({{8, 8 << 18}, {1, 8}})
    ->MeasureProcessCPUTime()
    ->UseRealTime();
```<lst:benchmark>

As we can see from the code above, the array is generated inside the benchmark function, which means that it is generated for each iteration of the benchmark. In order to address this issue, we moved the array generation outside of the benchmark function, hence generating it only once. The results of this change are the following:

// #TODO
