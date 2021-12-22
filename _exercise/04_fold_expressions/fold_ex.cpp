#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "catch.hpp"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename... Args>
decltype(auto) matches(const T& tab, const Args&... args)
{
    return (... + std::count(std::begin(tab), std::end(tab), args));
}

TEST_CASE("matches - returns how many items is stored in a container")
{
    // Tip: use std::count() algorithm

    vector<int> v{1, 2, 3, 4, 5};

    REQUIRE(matches(v, 2, 5) == 2);
    REQUIRE(matches(v, 100, 200) == 0);
    REQUIRE(matches("abccdef", 'x', 'y', 'z') == 0);
    REQUIRE(matches("abccdef", 'a', 'c', 'f') == 4);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

class Gadget
{
public:
    virtual std::string id() const { return "a"; }
    virtual ~Gadget() = default;
};

class SuperGadget : public Gadget
{
public:
    std::string id() const override
    {
        return "b";
    }
};

static_assert(std::is_same_v<std::common_type_t<int, int, char, double>, double>);

template <typename... TArgs>
std::vector<std::common_type_t<TArgs...>> make_vector(TArgs&&... args)
{
    std::vector<std::common_type_t<TArgs...>> vec;
    vec.reserve(sizeof...(args));

    (..., vec.push_back(std::forward<TArgs>(args)));

    return vec; // return l-value - may be optimized by RVO
}

namespace LegacyCode
{
    std::vector<int>* make_vector()
    {
        return new vector<int>(1'000'000);
    }
}

std::vector<int> make_vector_by_rvo()
{
    return std::vector{1, 2, 3, 4, 5}; // rvo is mandatory - return prvalue
}

TEST_CASE("rvo in C++17")
{
    std::vector<int> v = make_vector_by_rvo();
}

TEST_CASE("make_vector - create vector from a list of arguments")
{
    // Tip: use std::common_type_t<Ts...> trait

    using namespace Catch::Matchers;

    SECTION("ints")
    {
        std::vector<int> v = make_vector(1, 2, 3); // move constructor or RVO
        std::vector<int> v2 = {1, 2, 3};

        REQUIRE_THAT(v, Equals(vector{1, 2, 3}));
    }

    SECTION("unique_ptrs")
    {
        const vector<unique_ptr<int>> ptrs = make_vector(make_unique<int>(5), make_unique<int>(6));

        REQUIRE(ptrs.size() == 2);
    }

    SECTION("unique_ptrs with polymorphic hierarchy")
    {
        auto gadgets = make_vector(make_unique<Gadget>(), make_unique<SuperGadget>(), make_unique<Gadget>());

        static_assert(is_same_v<decltype(gadgets)::value_type, unique_ptr<Gadget>>);

        vector<string> ids;
        transform(begin(gadgets), end(gadgets), back_inserter(ids), [](auto& ptr) { return ptr->id(); });

        REQUIRE_THAT(ids, Equals(vector<string>{"a", "b", "a"}));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Range
{
    T low, high;
};

template <typename T1, typename T2>
Range(T1, T2) -> Range<std::common_type_t<T1, T2>>;

template <typename T, typename... TArgs>
bool within(const Range<T>& range, const TArgs&... args)
{
    auto in_range = [&range](const auto& value) { return value >= range.low && value <= range.high; };

    return (... && in_range(args));
}

TEST_CASE("within - checks if all values fit in range [low, high]")
{
    REQUIRE(within(Range{10, 20.0}, 1, 15, 30) == false);
    REQUIRE(within(Range{10, 20}, 11, 12, 13) == true);
    REQUIRE(within(Range{5.0, 5.5}, 5.1, 5.2, 5.3) == true);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
void hash_combine(size_t& seed, const T& value)
{
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename... TArgs>
size_t combined_hash(const TArgs&... args)
{
    size_t seed{};

    (..., hash_combine(seed, args));
    
    return seed;
}


template <typename... Ts>
size_t combined_hash_for_tuple(const std::tuple<Ts...>& tpl)
{
    auto hasher = [](const auto&... args) {
        return combined_hash(args...);
    };

    return std::apply(hasher, tpl);
}

TEST_CASE("combined_hash - write a function that calculates combined hash value for a given number of arguments")
{
    REQUIRE(combined_hash(1U) == 2654435770U);
    REQUIRE(combined_hash(1, 3.14, "string"s) == 10365827363824479057U);
    REQUIRE(combined_hash(123L, "abc"sv, 234, 3.14f) == 162170636579575197U);
}

struct Person
{
    std::string fname;
    std::string lname;
    uint8_t age;

    auto tied() const
    {
        return std::tie(fname, lname, age);
    }

    bool operator==(const Person& other) const 
    {
        return tied() == other.tied();
    }

    bool operator<(const Person& other) const
    {
        return tied() < other.tied();
    }

    std::size_t hash() const
    {
        return combined_hash_for_tuple(tied());
    }
};