#import "@preview/problemst:0.1.2": pset
#import "@preview/algorithmic:1.0.5"
#import algorithmic: style-algorithm, algorithm-figure
#show: style-algorithm

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

We started from the classic implementation of the Merge Sort:

```cpp
void parallel_merge_sort
  (std::vector<int>& arr, int left, int right, int depth) {

    if (left >= right) return;

    int mid = left + (right - left) / 2;

    if (depth < 4) { // limit parallel recursion depth
        #pragma omp parallel sections
        {
            #pragma omp section
            parallel_merge_sort(arr, left, mid, depth + 1);
            #pragma omp section
            parallel_merge_sort(arr, mid + 1, right, depth + 1);
        }
    } else {
        // fallback sequential recursion
        parallel_merge_sort(arr, left, mid, depth + 1);
        parallel_merge_sort(arr, mid + 1, right, depth + 1);
    }

    merge(arr, left, mid, right); // sequential merge
}
```

== Performance

In order to measure the performance of out implementation we used the tool *perf* on a Linux machine.
It's important to note that the performance in this case are highly dependent on the hardware and the specific implementation details. Moreover, we will run the comparison on the same machine to ensure a fair comparison, with the following specifications:
- CPU: <name>, <n> cores, <m> threads, <frequency> GHz
- RAM:
- OS: 

The results of our first implementation are the following:

```shell
perf stat ./out/basic-pmsort 100000000

 Performance counter stats for './out/basic-pmsort 100000000':

         17,730.01 msec task-clock:u                     #    1.587 CPUs utilized
                 0      context-switches:u               #    0.000 /sec
                 0      cpu-migrations:u                 #    0.000 /sec
           341,059      page-faults:u                    #   19.236 K/sec
    84,909,023,561      instructions:u                   #    1.28  insn per cycle
    66,094,024,554      cycles:u                         #    3.728 GHz
    17,209,658,611      branches:u                       #  970.651 M/sec
     1,292,855,293      branch-misses:u                  #    7.51% of all branches
                        TopdownL1                 #     20.9 %  tma_backend_bound
                                                  #     47.0 %  tma_bad_speculation
                                                  #     13.4 %  tma_frontend_bound
                                                  #     18.7 %  tma_retiring

      11.168861879 seconds time elapsed

      17.157796000 seconds user
       0.549208000 seconds sys
```

From the data collected, we can see that the parallel implementation is significantly faster than the sequential one, especially for such large arrays. The bottleneck seems to be caused by the sequential merge step summed with the low number of threads used.

= Parallel Merge

== Selection problem
The selection problem states: \
_"The input to our selection problem is the following: two lists sorted in increasing order, $A$ and $B$, and a
value $k$. The goal is to find two values $a$ and $b$ that are defined as follows. Let $C$ be the result of merging $A$
and $B$, increasingly. Recall that we have assumed that $C$ contains distinct elements. Consider the set of $k$
smallest values of $C$ and denote it with $C_k$; the index $a$ is defined as the number elements of $A$ contained in
$C_k$ (i.e., $a = |C_k inter A|$) and $b$ is defined similarly (i.e., $b = |C_k inter B|$)."_

Given $A$, $B$, the value of $k$, and an index $i$, show that in $O(1)$ time (using a single processor) we can check whether $a < i, a = i "or" a > i$ (without having to compute $C$). Based on this, show that selection can be solved in $O(log n)$ time using a single processor.


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
          $l <= r$,
          {
            Assign([a], FnInline[floor][$(l + r) / 2$])
            Assign([b], $k - a$)
            IfElseChain(
              $A[a + 1] < B[b]$,
              {
                Assign[$l$][$a + 1$]
              },
              [$A[a] > B[b+1]$],
              {
                Assign[$r$][$a - 1$]
              },
              Return[a, b],
            )
          },
        )
        Return[*null*]
      },
    )
  }
)


```cpp
std::pair<int, int> selection_problem(const std::vector<int>& A, const std::vector<int>& B, int k) {
    int n = A.size();
    int m = B.size();

    if (k >= n + m) return {n, m};
    if (k <= 0) return {0, 0};

    int a_min = std::max(0, k - m);
    int l = a_min;
    int r = std::min(k, n);

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