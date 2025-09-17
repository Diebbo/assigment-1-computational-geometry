CFLAGS := -O3
CPPFLAGS := -std=c++17 -fopenmp -g -I/opt/homebrew/include -O3
LDFLAGS := -L/opt/homebrew/lib

all: out/basic-pmsort out/bench

out/basic-pmsort: src/basic-pmsort.cpp src/main.cpp | out
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $^
out/bench: src/basic-pmsort.cpp src/bench.cpp | out
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -lbenchmark -o $@ $^

out:
	mkdir -p out

clean:
	rm -rf out
