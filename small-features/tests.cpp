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
    int values[4];

    void print() const
    {
        std::cout << "{ " << i << ", " << d << ", '" << s << "', [ ";
        for(const auto& v : values)
            std::cout << v << " ";
        std::cout << "] }\n";
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

    struct WTF
    {
        int value;

        WTF() = delete; // user declared
    };
}

TEST_CASE("aggregates")
{
    Aggregate1 agg1{1, 3.14, "text", {1, 2, 3}};
    agg1.print();

    Aggregate1 agg2{1};
    agg2.print();

    Aggregate1 agg3{};
    agg3.print();

    Cpp17::Aggregate2 agg4{{1, 3.14, "text"}, {1, 2, 3, 4}};
    agg4.print();
    REQUIRE(agg4.data == std::vector{1, 2, 3, 4});

    Cpp17::Data ds1{{"dataset"}, {1, 2, 3, 4}};

    Cpp17::WTF wtf{};
}

TEST_CASE("std::array is aggregate")
{
    std::array<int, 10> arr1 = {1, 2, 3};

    static_assert(std::is_aggregate_v<decltype(arr1)>);
}

/////////////////////////////////////////////////////////////
// attributes

struct ErrorCode
{
    int ec;
    const char* msg;
};

[[nodiscard]] ErrorCode open_file(const std::string& filename)
{
    if (filename == ""s)
        return {13, "Empty string as filename"};
    return {0, ""};
}

enum Coffee {
   espresso = 1,
   americano [[deprecated]] = espresso
};

namespace [[deprecated]] LegacyCode
{
    int value;
}

TEST_CASE("attributes")
{
    auto result = open_file("");

    auto [error_code, message] = open_file("");

    LegacyCode::value = 1;

    [[maybe_unused]] auto i = 0;
}