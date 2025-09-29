#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/merge_sort.hpp"

TEST_CASE( "Simple Merge Sort", "[merge_sort]" ) {
    std::vector<int> vec = {5, 3, 8, 6, 2};
    parallel_merge_sort(vec, 0, vec.size() - 1, 0);
    REQUIRE( vec == std::vector<int>{2, 3, 5, 6, 8} );
}
