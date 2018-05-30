#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "skiplist.h"
#include <vector>
#include <tuple>

TEST_CASE("skiplist") {
    SECTION("should init") {
        REQUIRE_NOTHROW([&]{
            skiplist<int, 8> sklist;
        });
    }

    SECTION("move constructor should not throw") {
        skiplist<int, 8> sklist;
        for (int i = 0; i < 100; i++) {
            sklist.insert(i);
        }
        REQUIRE_NOTHROW([&]{
            skiplist<int, 8> new_sklist(std::move(sklist));
        }());
    }

    SECTION("should be able to insert") {
        skiplist<int, 8> sklist;
        for (int i = 0; i < 10; i++) {
            sklist.insert(i);
        }
        std::vector<int> v1(sklist.begin(), sklist.end());
        std::vector<int> v2 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        REQUIRE( v1 == v2 );
    }

    SECTION("should copy") {
        skiplist<std::tuple<int, double>, 8> sklist1;
        for (int i = 0; i < 10; i++) {
            sklist1.emplace(i, .0 + i);
        }
        skiplist<std::tuple<int, double>, 8> sklist2(sklist1);
        int v = 0;
        for (auto const &t : sklist2) {
            REQUIRE( (std::get<0>(t) == v) );
            REQUIRE( (std::get<1>(t) == Approx(.0 + v)) );
            ++v;
        }

    }

    SECTION("can find the element") {
        skiplist<int, 8> sklist;
        sklist.insert(1);
        sklist.insert(2);
        REQUIRE( (sklist.find(3) == sklist.end()) );
        REQUIRE( ((*sklist.find(2)) == 2) );
        REQUIRE( ((*sklist.find(1)) == 1) );
        REQUIRE( ((*(++sklist.find(1))) == 2) );
        REQUIRE( (++sklist.end() == sklist.end()) );
    }

    SECTION("should erase correctly") {
        skiplist<int, 8> sklist = {1, 1, 2, 3, 4, 2, 1, 2, 3, 3, 1, 1, 5};
        REQUIRE( sklist.erase(1) == 5 );
        REQUIRE( sklist.erase(3) == 3 );
        std::vector<int> v1 = {2, 2, 2, 4, 5};
        std::vector<int> v2(sklist.begin(), sklist.end());
        REQUIRE( v1 == v2 );
    }

}
