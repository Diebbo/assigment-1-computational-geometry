CFLAGS := -O3
CPPFLAGS := -std=c++17 -fopenmp -g -I/opt/homebrew/include -O3
LDFLAGS := -L/opt/homebrew/lib

all: out/basic_pmsort

test: out/test_simple_parallel_merge

.PHONY: bench
bench: out/bench
	python3 src/scripts/array_generator.py -o /tmp/data.txt -n 100000
	out/bench --benchmark_out=./out/bench_result.json --benchmark_out_format=json --benchmark_filter=BM_MergeSort

out/basic_pmsort: src/basic_pmsort.cpp src/main.cpp src/util.cpp src/cli.cpp src/selection_problem.cpp src/parallel_merge.cpp src/fully_parallel_merge_sort.cpp | out
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

out/bench: src/basic_pmsort.cpp src/bench.cpp src/util.cpp src/cli.cpp | out
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -lbenchmark -o $@ $^

out/parallel_merge: src/basic_pmsort.cpp src/selection_problem.cpp src/util.cpp src/parallel_merge.cpp src/fully_parallel_merge_sort.cpp | out
	$(CXX) -o $@ $^ $(CPPFLAGS)

out/test_simple_parallel_merge: src/selection_problem.cpp src/parallel_merge.cpp src/util.cpp src/basic_pmsort.cpp test/test_simple_parallel_merge.cpp | out
	$(CXX) -o $@ $^ $(CPPFLAGS) -o $@ 

out:
	mkdir -p out

clean:
	rm -rf out
