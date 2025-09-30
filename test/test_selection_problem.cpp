#include "catch.hpp"
#include "../src/merge_sort.hpp"

TEST_CASE( "Selection Problem", "[selection]" ) {
    std::vector<int> A = {1, 3, 5, 7, 9};
    std::vector<int> B = {2, 4, 6, 8, 10};

    SECTION("k=0") {
        auto [a_count, b_count] = selection(A, B, 0);
        REQUIRE(a_count == 0);
        REQUIRE(b_count == 0);
    }

    SECTION("k=5") {
        auto [a_count, b_count] = selection(A, B, 5);
        REQUIRE(a_count + b_count == 5);
        REQUIRE(a_count == 3); // One possible valid split
        REQUIRE(b_count == 2);
    }

    SECTION("k=10") {
        auto [a_count, b_count] = selection(A, B, 10);
        REQUIRE(a_count == 5);
        REQUIRE(b_count == 5);
    }

    SECTION("k=7") { auto [a_count, b_count] = selection(A, B, 7);
        REQUIRE(a_count + b_count == 7);
        REQUIRE(a_count == 4); // One possible valid split
        REQUIRE(b_count == 3);
    }
}
