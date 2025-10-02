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

== Theoretical Background

Merge Sort is a divide and conquer algorithm that can be easily parallelized. The first step is to divide the array into two halves and recursively solve for each of them. To prove that is better then the sequential version, we need to make some calculation about the time complexity, depth and work of the algorithm.

- Work: $W(n) = 2W(n/2) + O(n) = O(n log n)$ from the Master Theorem, where $O(n)$ is the time taken to merge *sequentially* two sorted arrays and we have two recursive calls.
- Depth: $D(n) = D(n/2) + O(n) = O(n)$ where $O(n)$ is the time taken to merge *sequentially* two sorted arrays.


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

It's important to note that the performance in this case are highly dependent on the hardware and the specific implementation details. Moreover, we will run the comparison on the same machine to ensure a fair comparison, with the following specifications:
- CPU: Intel i5-1135G7, 4 cores, 8 threads, 4.2 GHz
- RAM: 8 GB
- OS: Arch Linux - Kernel 6.16.7-arch1-1

#figure(
  image("./BM_MergeSort.png", width: 90%),
  caption: "Comparison of the performance of the parallel merge sort with different number of threads",
)<fig:comp-4-cpus>

As a first test, we tried comparing the performance of our implementation on the same machine with different number of threads involved (see @fig:comp-4-cpus).

To achieve this, we used the #link("https://github.com/google/benchmark")[Google Benchmark library ], which is a microbenchmarking library that allows us to easily measure the performance of our code. The code in @lst:mergesort_benchmark_inside shows the benchmark implementation we used for this test.

The outcomes aligned with our expectations: as the number of threads increases, performance rises. Nonetheless, the rate of improvement gradually declines, clearly illustrating the phenomenon commonly referred to as "diminishing returns".

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

As we can see from the code in @lst:mergesort_benchmark_inside, the array is generated inside the for loop at line 6, meaning it is generated for each iteration of the benchmark. In order to address this issue, we moved the array generation outside of the benchmark loop, hence generating it only once.

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

In conclusion, we can say that the performance of the selection problem is $O(log(n + m))$ (this will be useful later on for the parallel merge).

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

  #pragma omp taskloop
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
  
  #pragma omp taskloop
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

The main idea is to divide the work among the available threads,
using the selection problem to find the indices where to split the two arrays. In order to do that, we initialize a support vector for each of the two arrays and one for the merged array, which will contain the indices where to split the arrays. Then, we use a parallel for loop to fill these vectors using the selection problem.

Finally, we use another parallel for loop to merge the two arrays, using the indices we found before because we need to sequentially merge the subarrays.

== Theoretical Considerations

The work of the algorithm is $W(n) = O(n)$, as for one CPU the cost is the same as a sequential merge. But the depth is $D(n) = O(log(n))$, since we cannot parallelize the selection problem, while the merging of the subarrays takes $O(n / p)$ time, where $p$ is the number of processors. Thus, having infinite processors the depth is dominated by the selection problem, which is $O(log(n))$.

In the real world, the number of processors is limited, hence the factor $n / p$ will have a big impact on the performance of the algorithm. Furthermore, we have that $T(n, p) = O(log(n) + n / p + epsilon(p))$, where $epsilon(p)$ is the overhead due to the parallelization with $p$ processors. This means that for small arrays, the overhead will dominate the performance of the algorithm, making it slower than a sequential merge.

== Benchmarking

For the reasons explained before, we found our benchmark to be slower then the sequential merge for small arrays. To solve this problem, we added a threshold to the parallel merge function, which will use a sequential merge for small arrays. We found that the optimal threshold is around $2^14$ elements.
The benchmark results can be seen in @fig:merge-sequential-vs-parallel.

#figure(
  caption: "Performance comparison between the sequential and parallel merge implementations",
  image("BM_SequentialMerge vs BM_ParallelMerge.png"),
) <fig:merge-sequential-vs-parallel>

= Fully Parallel Merge Sort

We can combine all the techniques explained so far to obtain a fully parallel merge sort algorithm.
The following code implements it:

```cpp
void fully_parallel_merge_sort(vector<int> &arr, int left, int right) {
  if (left >= right)
    return;

  int mid = left + (right - left) / 2;


#pragma omp taskgroup
    {
#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
      fully_parallel_merge_sort(arr, left, mid);
#pragma omp task shared(arr) untied if (right - left >= (1 << 14))
      fully_parallel_merge_sort(arr, mid + 1, right);
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
  image("./full_parallel_alone.png", width: 90%),
  caption: "Comparison of performance of the fully parallel merge sort with different number of threads",
)<fig:fully-parallel-merge-sort>

At the same time it's important to notice that in our performance graph (see @fig:fully-parallel-merge-sort) for some small inputs there's a plateau in performance. Let's zoom in on that part of the graph to understand better what's going on.

#figure(

  image("./fpms-cpus-comparison-random-array.png", width: 90%),
  caption: "comparison of performance of the fully parallel merge sort with different number of threads and std (log scale)",
)<fig:fully-parallel-merge-sort-sorted-zoomed>

We can see that for both single and multiple threads there's a huge gap in performance between our implementation and the standard library one. We were expecting this behavior, since the standard library implementation is highly optimized and uses various techniques to improve performance, such as insertion sort for small arrays and other low-level optimizations, while our implementation of the parallel merge is not optimized, expecially because using taskloop inside a parallelized recursive function seems counterproductive in OpenMP.

= New parallel merge

We tried implementing a new version of the parallel merge, which uses a different approach to divide the work among the threads. The idea is to use a recursive approach to divide the two arrays into smaller subarrays, until we reach a base case where we can merge the two subarrays sequentially.

```cpp
void new_parallel_merge(const std::vector<int> &A, int a_start, int a_end,
                        const std::vector<int> &B, int b_start, int b_end,
                        std::vector<int> &C, int c_start) {
  int a_len = a_end - a_start + 1;
  int b_len = b_end - b_start + 1;

  if (a_len < b_len) {
    return new_parallel_merge(B, b_start, b_end, A, a_start, a_end, C, c_start);
  }

  if (a_len == 0)
    return;

  if (a_len + b_len <= MERGE_THRESHOLD) {
    std::merge(A.begin() + a_start, A.begin() + a_end + 1, B.begin() + b_start,
               B.begin() + b_end + 1, C.begin() + c_start);
    return;
  }

  int a_mid = a_start + a_len / 2;
  int b_mid =
      std::lower_bound(B.begin() + b_start, B.begin() + b_end + 1, A[a_mid]) -
      B.begin();
  int c_mid = c_start + (a_mid - a_start) + (b_mid - b_start);

  C[c_mid] = A[a_mid];

#pragma omp task shared(A, B, C)
  new_parallel_merge(A, a_start, a_mid - 1, B, b_start, b_mid - 1, C, c_start);

#pragma omp task shared(A, B, C)
  new_parallel_merge(A, a_mid + 1, a_end, B, b_mid, b_end, C, c_mid + 1);

#pragma omp taskwait
}
```

We can see the benchmark in figure @fig:new-parallel-merge-sort, where we can see that the performance is better than the previous implementation and sequential merge.

#figure(
  image("./best_case_merge.png", width: 90%),
  caption: "Comparison of performance of the new parallel merge sort with different number of threads",
)<fig:new-parallel-merge-sort>

= Conclusion

In this project, we set out to design, implement, and analyze a fully parallel merge sort algorithm using C++ and OpenMP. Starting from the classical sequential merge sort, we progressively introduced parallelism first in the recursive decomposition phase and later in the merging step, eventually arriving at a fully parallel solution.

From a theoretical standpoint, we analyzed the work and depth of the parallel merge sort. While the total work remains $O(n log n)$—identical to the sequential algorithm—the parallel version achieves a depth of $O(log^2 n)$ in the fully parallelized version, with the merge step itself having depth $O(log n)$. This highlights the potential speedup achievable when sufficient hardware resources are available, while also clarifying the inherent limitations due to factors such as synchronization and sequential components (e.g., selection sub problems).

On the implementation side, we experimented with multiple strategies to introduce parallelism. Our first approach applied OpenMP tasks directly to recursive calls, followed by a task-based parallel merge relying on the selection problem. We then explored an alternative recursive parallel merge that allowed for better task granularity and more efficient use of threads. These explorations demonstrated both the strengths and pitfalls of OpenMP in recursive algorithms, in particular the overhead introduced by taskloop constructs in deeply nested calls.

The benchmarking phase provided valuable insights. Using Google Benchmark on a controlled hardware setup, we compared sequential, partially parallel, and fully parallel implementations. The results confirmed the theoretical predictions:

For small input sizes, the overhead of task creation and synchronization outweighed the benefits of parallelism, making the sequential version faster.

For medium to large input sizes, parallelism yielded significant speedups, though with diminishing returns as the number of threads increased—a consequence of Amdahl’s law and the overhead $epsilon(p)$ associated with parallel execution.

When compared against the standard library sort, our implementation performed worse across most scenarios. This was expected, given that the standard library leverages decades of optimization, hybrid strategies (e.g., insertion sort for small partitions), and low-level performance tuning, whereas our focus was on correctness and parallelism rather than fine-grained optimization.

Overall, the project demonstrates how parallel computing can be applied to classical algorithms, offering both practical improvements in certain scenarios and a deeper understanding of the trade-offs involved. While our implementation cannot compete with highly optimized production-level algorithms, it fulfills its role as a pedagogical exploration of algorithmic parallelization.

Looking ahead, several improvements and extensions could be considered. These include tuning thresholds for hybrid sequential/parallel execution, experimenting with work-stealing schedulers to better balance load across threads, and investigating SIMD or GPU-based implementations for additional performance gains.

In conclusion, this work illustrates not only the potential but also the challenges of parallel algorithm design: careful consideration of theoretical complexity, hardware constraints, and parallelization overheads is essential to achieving meaningful improvements.
