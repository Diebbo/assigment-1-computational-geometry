.SUFFIXES:
.PHONY: all, clean, tests, bench

CPPFLAGS += -std=c++17 -fopenmp -O3
LDFLAGS += -fopenmp -lbenchmark

SRCS      = $(wildcard src/*.cpp)
TESTSRCS  = $(wildcard test/*.cpp)
OBJS      = $(patsubst %.cpp, out/%.o, $(SRCS))
TESTOBJS  = $(patsubst test/%.cpp, out/test/%.o, $(TESTSRCS))
EXECS     = out/bench out/main out/tests

# === PHONY TARGETS ===
all: $(EXECS)

clean:
	rm -rf out

tests: out/tests
	out/tests

bench: out/bench
	python3 src/scripts/array_generator.py -o /tmp/data.txt -n 2097152
	out/bench --benchmark_out=./out/bench_result_fpms.json --benchmark_out_format=json --benchmark_filter=BM_FullyParallelMergeSort
	out/bench --benchmark_out=./out/bench_result_spms.json --benchmark_out_format=json --benchmark_filter=BM_MergeSort
	python3 src/scripts/array_generator.py -o /tmp/sorted.txt -n 2097152 -s
	out/bench --benchmark_out=./out/bench_result_fpms_sorted.json --benchmark_out_format=json --benchmark_filter=BM_Selection

# === Executables ===
out/bench: $(OBJS) out/src/bin/bench.o
	$(CXX) $(LDFLAGS) -o $@ $^
out/main: $(OBJS) out/src/bin/main.o
	$(CXX) $(LDFLAGS) -o $@ $^
out/tests: $(OBJS) $(TESTOBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

# === Objects ===
out/src/%.o: src/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $^
out/test/%.o: test/%.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -c -o $@ $^
