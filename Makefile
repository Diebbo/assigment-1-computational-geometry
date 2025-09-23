CFLAGS := -O3
CPPFLAGS := -std=c++17 -fopenmp -g -I/opt/homebrew/include -O3
LDFLAGS := -L/opt/homebrew/lib

all: out/basic-pmsort out/bench

.PHONY: bench
bench: out/bench
	python3 src/scripts/array_generator.py -o /tmp/data.txt -n 100000
	out/bench --benchmark_out=./out/bench_result.json --benchmark_out_format=json

out/basic-pmsort: src/basic-pmsort.cpp src/main.cpp src/util.cpp | out
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^
out/bench: src/basic-pmsort.cpp src/bench.cpp src/util.cpp | out
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -lbenchmark -o $@ $^

out:
	mkdir -p out

clean:
	rm -rf out
