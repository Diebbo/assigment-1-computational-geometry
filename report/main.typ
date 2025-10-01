#import "@preview/problemst:0.1.2": pset
#import "@preview/algorithmic:1.0.5"
#import algorithmic: algorithm-figure, style-algorithm
#show: style-algorithm

#show figure.where(kind: raw): set align(left)
#show figure.caption: set align(center)

#show: pset.with(
  class: "Computational geometry",
  student: "",
  title: "Parallel Merge Sort",
  date: datetime.today(),
  collaborators: ("Diego Barbieri", "Samuele Esposito", "Gioele Scandaletti"),
)

= Basic Parallel Merge Sort

Firstly, we need to define the tech stack we are going to use for this project. We choose to use *C++* with the library *OpenMP* for parallelization. The reason behind this choice is the high performance and the low level control that the language offers.

== Implementation

We started from the classic implementation of the Merge Sort algorithm, which is a divide and conquer algorithm that works by recursively dividing the array into two halves, sorting each half, and then merging the two sorted halves back together. We then added the following `#pragma` directives:

```cpp
void parallel_merge_sort(vector<int> &arr, int left, int right, int depth) {
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

#figure(
  image("./BM_MergeSort.png", width: 90%),
  caption: "Comparison of the performance of the parallel merge sort with different number of threads",
)<fig:comp-4-cpus>

As a first test, we tried comparing the performance of our implementation on the same machine with different number of threads involved (see @fig:comp-4-cpus). The results followed our expectations: as we increase the number of threads, the performance improves. However, we can see that the improvement is decreasing, meaning we have a clear case of what is usually called "diminishing returns".

#figure(caption: "Benchmark of our Parallel Merge Sort implementation.")[
  ```cpp
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
  ```
] <lst:mergesort_benchmark_inside>

As we can see from the code in @lst:mergesort_benchmark_inside, the array is generated inside the for loop at line 6, meaning it is generated for each iteration of the benchmark. In order to address this issue, we moved the array generation outside of the benchmark loop, hence generating it only once. The results of this change can be seen in @lst:mergesort_benchmark_outside.

#figure(caption: [Benchmark of our Parallel Merge Sort implementation,
  with input generation happening only once outside of the loop.
])[
  ```cpp
  static void BM_MergeSort_Optimized(benchmark::State &state) {
    // Initialize the vector
    vector<int> arr = init_default_vector(state.range(0));

    omp_set_max_active_levels(32);
    omp_set_num_threads(state.range(1));

    // Run the benchmark
    for (auto _ : state) {
      // Copy the input, and operate on that.
      vector<int> arr2(arr);
      #pragma omp parallel
      {
        #pragma omp single
        parallel_merge_sort(arr2, 0, arr2.size() - 1, 0);
      }
      benchmark::DoNotOptimize(arr2);
    }
  }
  ```] <lst:mergesort_benchmark_outside>

// #TODO
From the data collected, we can see that the parallel implementation is significantly faster than the sequential one, especially for very large arrays. The bottleneck seems to be caused by the sequential merge step summed with the low number of threads used.

= Parallel Merge

== Selection problem
The selection problem states: \
_"The input to our selection problem is the following: two lists sorted in increasing order, $A$ and $B$, and a
  value $k$. The goal is to find two values $a$ and $b$ that are defined as follows. Let $C$ be the result of merging $A$
  and $B$, increasingly. Recall that we have assumed that $C$ contains distinct elements. Consider the set of $k$
  smallest values of $C$ and denote it with $C_k$; the index $a$ is defined as the number elements of $A$ contained in
  $C_k$ (i.e., $a = |C_k inter A|$) and $b$ is defined similarly (i.e., $b = |C_k inter B|$)."_

== Solving selection
/*
Given $A$, $B$, the value of $k$, and an index $i$, show that in $O(1)$ time (using a single processor) we can check whether $a < i, a = i "or" a > i$ (without having to compute $C$). Based on this, show that selection can be solved in $O(log n)$ time using a single processor.
*/

Given $A$, $B$, the value of $k$, and an index $i$, we can check whether $a < i, a = i "or" a > i$ in $O(1)$ time, using a single processor and without having to compute $C$.

We can do this by observing the following (with $A[1]$ being the first element of $A$):
1. If $A[i] > B[k - i + 1]$, then the biggest element of $A$ included in the hypothetical $C_k$ would be smaller than the smallest element of $B$ excluded from $C_k$. Therefore, $a < i$.
2. If $A[i + 1] < B[k - i]$, then the smallest element of $A$ excluded from the hypothetical $C_k$ would be bigger than the biggest element of $B$ included in $C_k$. Therefore, $a > i$.
3. If neither of the two previous conditions hold, then the biggest element of $A$ included in the hypothetical $C_k$ is smaller than the biggest element of $B$ excluded from $C_k$, and the smallest element of $A$ excluded from $C_k$ is bigger than the smallest element of $B$ included in $C_k$. Therefore, $a = i$.

This is the general idea to check the relation between $a$ and $i$ in constant time, with just at most two comparisons.

We can use this idea to solve the selection problem in $O(log n)$ time using a single processor, by performing a binary search on the possible values of $a$. We start with the range $[max(0, k - m), min(k, n)]$ (where $n$ and $m$ are the sizes of $A$ and $B$ respectively) and repeatedly halve the range based on the result of the check described above, until we find the correct value of $a$. The corresponding value of $b$ can then be computed as $b = k - a$.

#algorithm-figure(
  "Selection problem",
  vstroke: .5pt + luma(200),
  {
    import algorithmic: *
    Procedure(
      "Selection-Problem",
      ("A", "B", "k"),
      {
        Assign[$l$][$1$]
        Assign[$r$][$n$]
        LineBreak
        While(
          $l < r$,
          {
            Assign([a], FnInline[floor][$(l + r) / 2$])
            Assign([b], $k - a$)
            IfElseChain(
              $A[a] > B[b+1]$,
              {
                Assign[$r$][$a - 1$]
              },
              $A[a + 1] < B[b]$,
              {
                Assign[$l$][$a + 1$]
              },
              Return[a, b],
            )
          },
        )
        Return[l, k-l]
      },
    )
  },
) <selection-problem-alg>

The code in @lst:selection-problem-cpp implements @selection-problem-alg
in C++, considering the edge cases as well.

#figure(caption: "Selection problem implementation in C++")[
  ```cpp
  pair<int, int> selection_problem(vector<int> &A, vector<int> &B, int k) {
      int n = A.size();
      int m = B.size();

      if (k >= n + m) return {n, m};
      if (k <= 0) return {0, 0};

      int a_min = max(0, k - m);
      int l = a_min;
      int r = min(k, n);

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
  ```
] <lst:selection-problem-cpp>

== Fully Parallel Merging

Using the method explained in the assignment description,
we can merge two sorted arrays in parallel.
The following code in C++ implements that method:

```cpp
void parallel_merge(vector<int> &A, vector<int> &B,
                    vector<int> &C, int offset) {
  int n = A.size() + B.size();
  int num_threads = omp_get_max_threads();

  if (n < num_threads)
    num_threads = n;

  vector<int> a_indices(num_threads + 1);
  vector<int> b_indices(num_threads + 1);
  vector<int> k_indices(num_threads + 1);
  a_indices[0] = 0;
  b_indices[0] = 0;
  k_indices[0] = 0;

  #pragma omp parallel for
  for (int i = 1; i < num_threads; i++) {
    int k = i * n / num_threads;
    auto [a_count, b_count] = selection(A, B, k);
    a_indices[i] = a_count;
    b_indices[i] = b_count;
    k_indices[i] = k;
  }

  a_indices[num_threads] = A.size();
  b_indices[num_threads] = B.size();
  k_indices[num_threads] = n;

  #pragma omp parallel for
  for (int i = 0; i < num_threads; i++) {
    int a_end = a_indices[i + 1];
    int b_end = b_indices[i + 1];

    int a_idx = a_indices[i];
    int b_idx = b_indices[i];
    int k_idx = k_indices[i] + offset;

    while (a_idx < a_end && b_idx < b_end) {
      if (A[a_idx] <= B[b_idx])
        C[k_idx++] = A[a_idx++];
      else
        C[k_idx++] = B[b_idx++];
    }
    while (a_idx < a_end)
      C[k_idx++] = A[a_idx++];
    while (b_idx < b_end)
      C[k_idx++] = B[b_idx++];

  }
}
```

== Benchmarking

When we ran some benchmarks on our parallel merge implementation,
we found it to be _slower_ than the sequential implementation.
We suppose this could be due to synchronization overhead, or overhead in OpenMP,
however we were not able to prove our intuition.
The benchmark results can be seen in @fig:merge-sequential-vs-parallel.

#figure(
  caption: "Performance comparison between the sequential and parallel benchmark implementations",
  image("BM_SequentialMerge vs BM_ParallelMerge.png"),
) <fig:merge-sequential-vs-parallel>

= Fully Parallel Merge Sort

We can combine all the techniques explained so far to obtain a fully parallel merge sort algorithm.
The following code implements it:

```cpp
void fully_parallel_merge_sort(vector<int> &arr, int left, int right,
                               int depth) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;

  // limit parallel recursion depth based on machine capabilities
  if (depth < 5) {
#pragma omp taskgroup
    {
#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
      fully_parallel_merge_sort(arr, left, mid, depth + 1);
#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
      fully_parallel_merge_sort(arr, mid + 1, right, depth + 1);
    }
  } else {
    // fallback sequential recursion
    fully_parallel_merge_sort(arr, left, mid, depth + 1);
    fully_parallel_merge_sort(arr, mid + 1, right, depth + 1);
  }
  // create the two vectors to merge
  vector<int> left_vec;
  vector<int> right_vec;
  left_vec.assign(arr.begin() + left, arr.begin() + mid + 1);
  right_vec.assign(arr.begin() + mid + 1, arr.begin() + right + 1);

  parallel_merge(left_vec, right_vec, arr, left);
}
```

== Benchmarking

To test the performance of our implementation, we used the same benchmarking framework as before,
to compare it with the previous parallel implementation and the algorithm from the standard library.

#figure(
  image("./fpms-cpus-comparison-random-array.png", width: 90%),
  caption: "Comparison of performance of the fully parallel merge sort with different number of threads",
)<fig:fully-parallel-merge-sort>

At first glance, we can observe that the performance seems to respect our expectation reaching an order of magnitude of improvement compared to the algorithm.

#figure(
  image("./BM_StdSort.png", width: 90%),
  caption: "standard library sort performance",
)<fig:fully-parallel-merge-sort-sorted>

At the same time it's important to notice that in our performance graph (see @fig:fully-parallel-merge-sort-sorted) for some small inputs there's a plateau in performance. Let's zoom in on that part of the graph to understand better what's going on.

#figure(
  grid(
    columns: 2,
    // 2 means 2 auto-sized columns
    gutter: 2mm,
    // space between columns
    image("./smaller-resoults-fully-parallel.png"), image("./smaller-resoults-standard-sort.png"),
  ),
  caption: "standard library sort performance zoomed in",
)<fig:fully-parallel-merge-sort-sorted-zoomed>

We can see that for both single and multiple threads there's a huge gap in performance between our implementation and the standard library one. We were expecting this behavior, since the standard library implementation is highly optimized and uses various techniques to improve performance, such as insertion sort for small arrays and other low-level optimizations.

= Conclusion

We encountered some difficulties in the implementation of actually parallel algorithm,
mainly due to the complexity of OpenMP, the framework we chose.
In the end, we still managed to implement a fully-parallel Merge Sort implementation
that beats the `std::sort` implementation found in `libstdc++`, the GNU C++ standard library.
