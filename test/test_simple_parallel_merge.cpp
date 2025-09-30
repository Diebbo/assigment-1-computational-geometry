#include "catch.hpp"
#include "../src/merge_sort.hpp"

TEST_CASE( "Simple Merge Sort", "[merge_sort]" ) {
    SECTION("Already sorted array") {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        parallel_merge_sort(vec, 0, vec.size() - 1, 0);
        REQUIRE( vec == std::vector<int>{1, 2, 3, 4, 5} );
    }
    SECTION("Reverse sorted array") {
        std::vector<int> vec = {5, 4, 3, 2, 1};
        parallel_merge_sort(vec, 0, vec.size() - 1, 0);
        REQUIRE( vec == std::vector<int>{1, 2, 3, 4, 5} );
    }
    SECTION("Random array") {
        int n = 100;
        std::vector<int> vec(n);
        std::generate(vec.begin(), vec.end(), []() { return rand() % 1000; });
        parallel_merge_sort(vec, 0, vec.size() - 1, 0);
        REQUIRE( std::is_sorted(vec.begin(), vec.end()) );
    }
    SECTION("Empty array") {
        std::vector<int> vec;
        parallel_merge_sort(vec, 0, vec.size() - 1, 0);
        REQUIRE( vec.empty() );
    }
}
