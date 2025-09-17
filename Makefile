CPPFLAGS := -L/opt/homebrew/opt/libomp/lib -I/opt/homebrew/opt/libomp/include -std=c++17 -O3

out/basic-pmsort: src/basic-pmsort.cpp src/main.cpp | out
	$(CXX) -o $@ $^ $(CPPFLAGS)

out:
	mkdir -p out

clean:
	rm -rf out
