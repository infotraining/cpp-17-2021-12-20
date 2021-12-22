#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>
#include <atomic>
#include <charconv>

#include "catch.hpp"

using namespace std;

TEST_CASE("optional")
{
    std::optional<int> o1;
    REQUIRE(o1.has_value() == false);

    std::optional<int> o2 = std::nullopt;
    REQUIRE(o2 == std::nullopt);

    SECTION("null for pointers")
    {
        int* ptr = nullptr;
        if (ptr != nullptr)
            std::cout << *ptr << "\n";
        
        if (ptr)
        {
            std::cout << *ptr << "\n";
        }
    }

    SECTION("null for other types")
    {
        std::optional<int> opt = std::nullopt;

        if (opt != std::nullopt)
            std::cout << *opt << "\n";

        if (opt)
        {
            std::cout << *opt << "\n";
        }
    }

    SECTION("access")
    {
        std::optional<int> opt{42};

        SECTION("unsafe - fast")
        {
            REQUIRE(*opt == 42);

            //opt.reset();
            opt = std::nullopt;

            //*opt = 42; // Beware - UB
        }

        SECTION("safe - slower")
        {
            std::optional opt{665};

            REQUIRE(opt.value() == 665);

            opt.reset();

            REQUIRE_THROWS_AS(opt.value(), std::bad_optional_access);
        }

        SECTION("value_or")
        {
            std::optional<std::string> name;

            REQUIRE(name.value_or("(not-set)") == "(not-set)"s);
        }
    }

    SECTION("construction in-place")
    {
        std::optional<std::atomic<int>> opt_atomic{std::in_place, 42};
        REQUIRE(opt_atomic.value().load() == 42);
    }

    SECTION("move semantics")
    {
        std::optional opt_str = "text"s;

        std::string target = std::move(*opt_str);

        REQUIRE(target == "text"s);

        REQUIRE(opt_str.has_value() == true);
    }
}

TEST_CASE("weird stuff")
{
    std::optional<bool> flag{false};

    if (!flag) // yields false
    {
    }

    if (flag) // yields true
    {
    }

    if (flag == false) // yields true
    {
    }
}

[[nodiscard]] std::optional<int> to_int(std::string_view str)
{
    int value;

    auto start = str.data();
    auto end = str.data() + str.size();

    if (auto [pos_end, error_code] = std::from_chars(start, end, value); error_code != std::errc{} || pos_end != end)
    {
        return std::nullopt;
    }

    return value;
}


TEST_CASE("to_int")
{
    optional<int> number = to_int("42");
    REQUIRE(number.value() == 42);

    number = to_int("44as");
    REQUIRE(number.has_value() == false);    
}

