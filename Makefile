CPPFLAGS := -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -std=c++17 -fopenmp -g

out/main: src/basic-pmsort.cpp src/selection-problem.cpp src/parallel-merge.cpp src/fully-parallel-merge-sort.cpp src/main.cpp | out
	$(CXX) -o $@ $^ $(CPPFLAGS)

out:
	mkdir -p out

clean:
	rm -rf out
