#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <array>

#include "catch.hpp"
#include "gadget.hpp"

using namespace std;

TEST_CASE("static inline")
{
    Gadget g1;
    REQUIRE(g1.id() == 1);
    
    Gadget g2;
    REQUIRE(g2.id() == 2);
}

////////////////////////////////////////////////
// Aggregates

struct Aggregate1
{
    int i;
    double d;
    std::string s;

    void print() const
    {
        std::cout << "{ " << i << ", " << d << ", '" << s << "' }\n";
    }
};

namespace Cpp17
{
    struct Aggregate2 : Aggregate1
    {
        std::vector<int> data;
    };

    struct Data : std::string
    {
        std::vector<int> data;
    };
}

TEST_CASE("aggregates")
{
    Aggregate1 agg1{1, 3.14, "text"};
    agg1.print();

    Aggregate1 agg2{1};
    agg2.print();

    Aggregate1 agg3{};
    agg3.print();

    Cpp17::Aggregate2 agg4{{1, 3.14, "text"}, {1, 2, 3, 4}};
    agg4.print();
    REQUIRE(agg4.data == std::vector{1, 2, 3, 4});

    Cpp17::Data ds1{{"dataset"}, {1, 2, 3, 4}};
}

TEST_CASE("std::array is aggregate")
{
    std::array<int, 10> arr1 = {1, 2, 3};

    static_assert(std::is_aggregate_v<decltype(arr1)>);
}