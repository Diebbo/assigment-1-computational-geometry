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
