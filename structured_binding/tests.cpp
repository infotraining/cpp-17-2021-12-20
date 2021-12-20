#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

#include "catch.hpp"

using namespace std;
using namespace std::literals;

namespace Cpp98
{
    void calc_stats(const vector<int>& data, int& min, int& max, double& avg)
    {

        std::pair<std::vector<int>::const_iterator, std::vector<int>::const_iterator> minmax_pos = minmax_element(data.begin(), data.end());
        min = *minmax_pos.first;
        max = *minmax_pos.second;

        avg = accumulate(data.begin(), data.end(), 0.0) / data.size();
    }
}

TEST_CASE("tuples in C++11")
{
    std::tuple<int, double, std::string> tpl {1, 3.14, "text"};

    REQUIRE(std::get<0>(tpl) == 1);
    REQUIRE(std::get<2>(tpl) == "text");

    auto another_tpl = std::make_tuple(1, 3.14, "text");

    REQUIRE(tpl == another_tpl);

    int x;
    double pi;
    std::string text;

    std::tuple<int&, double&, std::string&> ref_tpl {x, pi, text};
    ref_tpl = another_tpl;

    std::tie(x, pi, std::ignore) = tpl;

    REQUIRE(x == 1);
    REQUIRE(pi == Approx(3.14));
    REQUIRE(text == "text");
}

tuple<int, int, double> calc_stats(const vector<int>& data)
{
    vector<int>::const_iterator min, max;
    tie(min, max) = minmax_element(data.begin(), data.end());

    double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

    return make_tuple(*min, *max, avg);
}

namespace Cpp17
{
    template <typename TContainer>
    tuple<int, int, double> calc_stats(const TContainer& data)
    {
        const auto [min_pos, max_pos] = minmax_element(begin(data), end(data)); // structured bindings

        double avg = accumulate(begin(data), end(data), 0.0) / size(data); // std::size()

        return tuple(*min_pos, *max_pos, avg); // CTAD
    }
}

TEST_CASE("Before C++17")
{
    vector<int> data = {4, 42, 665, 1, 123, 13};

    SECTION("without tie")
    {
        auto result = calc_stats(data);

        REQUIRE(std::get<0>(result) == 1);
    }

    int min, max;
    double avg;

    tie(min, max, avg) = calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

TEST_CASE("Since C++17")
{
    int data[] = {4, 42, 665, 1, 123, 13};

    auto [min, max, avg] = Cpp17::calc_stats(data);

    REQUIRE(min == 1);
    REQUIRE(max == 665);
    REQUIRE(avg == Approx(141.333));
}

std::array<int, 3> get_coord()
{
    return std::array {1, 2, 3};
}

struct ErrorCode
{
    int ec;
    const char* m;
};

ErrorCode open_file(const char* filname)
{
    return ErrorCode {13, "Error#13"};
}

TEST_CASE("structured bindings - features")
{
    SECTION("native arrays")
    {
        int coords[] = {1, 2, 3};

        auto [x, y, z] = coords;

        REQUIRE(x == 1);
        REQUIRE(z == 3);
    }

    SECTION("std::pair")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

        auto [pos, was_inserted] = dict.insert(std::pair(3, "three"));

        REQUIRE(was_inserted == true);
    }

    SECTION("std::tuple")
    {
        auto [min, max, avg] = Cpp17::calc_stats(std::vector {1, 2, 4});

        REQUIRE(min == 1);
    }

    SECTION("std::array")
    {
        auto [x, y, z] = get_coord();

        REQUIRE(z == 3);
    }

    SECTION("struct/class")
    {
        auto [error_code, error_message] = open_file("bad file name");

        REQUIRE(error_code == 13);
        REQUIRE(error_message == "Error#13"s);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structure bindings - how it works")
{
    Timestamp t1 {11, 17, 0};

    const auto& [hours, minutes, seconds] = t1;

    SECTION("is interpreted as")
    {
        const auto& entity = t1;
        auto& hours = entity.h;
        auto& minutes = entity.m;
        auto& seconds = entity.s;
    }
}

TEST_CASE("Use cases")
{
    SECTION("iteration over map")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "two"}};

        SECTION("Before C++17")
        {
            for (const auto& kv : dict)
            {
                std::cout << kv.first << " - " << kv.second << "\n";
            }
        }

        for (const auto& [key, value] : dict)
        {
            std::cout << key << " - " << value << "\n";
        }
    }

    SECTION("for + multiple declarations")
    {
        std::cout << "\n-----------------\n";
        std::list lst{"one"s, "two"s, "three"s};

        for(auto [index, it] = std::tuple(0u, begin(lst)); it != end(lst); ++index, ++it)
        {
            std::cout << index << " - " << *it << "\n";
        }
    }
}

enum Something
{
    some,
    thing
};

const std::map<Something, std::string> something_desc = { { some, "some"}, {thing, "thing" } };

////////////////////////////////////
// tuple-like protocol for Something

// step 1 - std::tuple_size<Something>
template <>
struct std::tuple_size<Something>
{
    static constexpr size_t value = 2;
};

// step 2 - std::tuple_element<Index, Something>

template <>
struct std::tuple_element<0, Something>
{
    using type = std::underlying_type_t<Something>;
};

template <>
struct std::tuple_element<1, Something>
{
    using type = std::string;
};

// step 3 - get<Index>
template <size_t Index>
decltype(auto) get(Something sth);

template <>
decltype(auto) get<0>(Something sth)
{
    return static_cast<int>(sth);
}

template <>
decltype(auto) get<1>(Something sth)
{
    return something_desc.at(sth);
}

TEST_CASE("tuple-like protocol")
{
    Something sth = some;

    const auto [value, description] = sth;

    REQUIRE(value == 0);
    REQUIRE(description == "some"s);
}