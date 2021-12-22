#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item)
            {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
}

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

template <typename... TArgs>
auto sum(const TArgs&... args) // sum(1, 2, 3, 4)
{
    return (... + args); // left-fold expression: return (((1 + 2) + 3) + 4);
}

template <typename... TArgs>
auto sum_r(const TArgs&... args) // sum(1, 2, 3, 4)
{
    return (args + ...); // right-fold expression: return (1 + (2 + (3 + 4)));
}

template <typename... TArgs>
void print(const TArgs&... args) // print(1, 3.14, "text")
{
    auto with_space = [is_first = true](const auto& item) mutable
    {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        return item;
    };

    (std::cout << ... << (with_space(args))) << "\n"; // (((std::cout << 1) << 3.14) << "text")
}

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
    (..., (std::cout << args << "\n"));
}

template <typename... TArgs>
auto do_sth(const TArgs&... args)
{
    return (..., args);
}

TEST_CASE("comma operator in fold")
{
    REQUIRE(do_sth(1, 2, 3, 4, 5) == 5);
}

template <typename... TArgs>
void read_from_stream(TArgs&... args)
{
    (std::cin >> ... >> args);

    print_lines(args...);
}

template <typename... TArgs>
bool all_true(TArgs... args)
{
    return (... && args);
}

TEST_CASE("fold + variadic templates")
{
    using namespace BeforeCpp17;

    REQUIRE(sum(1, 2, 3, 4) == 10);

    print(1, 3.14, "text");

    // int x, y, z;
    // read_from_stream(x, y, z);

    REQUIRE(all_true() == true);
}

/////////////////////////////////
// invoke + call_foreach

template <typename F, typename... TArgs>
decltype(auto) invoke(F&& f, TArgs&&... args)
{
    return f(std::forward<TArgs>(args)...);
}

int my_add(int a, int b)
{
    return a + b;
}

template <typename F, typename... TArgs>
decltype(auto) call_foreach(F&& f, TArgs&&... args)
{
    return (..., invoke(f, std::forward<TArgs>(args)));
}

template <typename T, typename... Args>
auto poly(T x, Args&&... args)
{
    T xx = 0;
    return (... + ((xx == 0 ? xx = 1 : xx *= x) * args));
}

TEST_CASE("invoke + call_foreach")
{
    REQUIRE(invoke(my_add, 1, 4) == 5);

    auto printer = [](const auto& item)
    { std::cout << item << "\n"; };

    call_foreach(printer, 1, 4, "text", 3.14);
}