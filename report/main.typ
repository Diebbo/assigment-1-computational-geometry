#import "@preview/problemst:0.1.2": pset

#show: pset.with(
  class: "Computational geometry",
  student: "",
  title: "Parallel Merge Sort",
  date: datetime.today(),
  collaborators: ("Diego Barbieri", "Samuele Esposito", "Gioele Scandaletti"),
)

#let deriv(num, dnm) = [$ (d num) / (d dnm) $]

= Definition of the derivative
Something something infinitesimals something something. We can then define the derivative as the limit of the difference quotient as $Delta x arrow 0$:
$ deriv(f(x), x)&= lim_(Delta x arrow 0) (f(x + Delta x) - f(x)) / (Delta x). $

== Code!
```go
import "fmt"

func main() {
  fmt.Println("python sux!!1!")
}
```

=== Subproblem
We can nest subproblems!

==== Subsubproblem
As far as we want!

#pagebreak()

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

aaa
