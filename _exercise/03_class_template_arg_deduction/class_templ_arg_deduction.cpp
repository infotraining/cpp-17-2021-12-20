#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <optional>
#include <array>

#include "catch.hpp"

using namespace std;

template <typename T>
struct Holder
{
    T value;

    Holder(const T& v)
        : value(v)
    {
    }

    template <typename Args>
    Holder(initializer_list<Args> lst)
        : value(lst)
    {
    }
};

template <typename Args>
Holder(initializer_list<Args>)->Holder<vector<Args>>;

template <typename T>
Holder(T)->Holder<T>;

Holder(const char*)->Holder<string>;

///////////////////////////////////////////////////////////////////////////////////

TEST_CASE("Guess deduced argument types")
{
    int x = 42;
    int& ref_x = x;
    const int& cref_x = x;

    using TODO = void;

    Holder h1(x);
    static_assert(is_same_v<decltype(h1), Holder<int>>);

    Holder h2(ref_x);
    static_assert(is_same_v<decltype(h2), Holder<int>>);

    Holder h3(cref_x);
    static_assert(is_same_v<decltype(h3), Holder<int>>);

    int tab[10];
    Holder h4 = tab;
    static_assert(is_same_v<decltype(h4), Holder<int*>>);

    Holder h5("test");
    static_assert(is_same_v<decltype(h5), Holder<std::string>>);

    Holder h6{1, 2, 3, 4, 5, 6};
    static_assert(is_same_v<decltype(h6), Holder<vector<int>>>);

    Holder h7(h6);
    static_assert(is_same_v<decltype(h7), Holder<vector<int>>>);

    Holder h8{h6, h7};
    static_assert(is_same_v<decltype(h8), Holder<std::vector<Holder<vector<int>>>>>);
}

int foo(int, int)
{
    return 42;
}

TEST_CASE("CTAD - standard library")
{
    SECTION("std::pair")
    {
        int tab1[10] = {1, 2, 3};

        std::pair p1{tab1, "Text"}; // std::pair<int*, const char*>
        std::pair p2{"text"s, 3.14}; // std::pair<std::string, double>
    }

    SECTION("std::tuple")
    {
        std::tuple tpl{1, 3.14, "text", "text"s}; // std::tuple<int, double, const char*, std::string>

        int tab1[10] = {1, 2, 3};
        std::pair p1{tab1, "Text"}; // std::pair<int*, const char*>

        std::tuple t1 = p1; // std::tuple<int*, const char*>
        std::tuple t2{t1};  // std::tuple<int*, const char*>
    }

    SECTION("std::optional")
    {
        std::optional opt_value{42}; // optional<int>
        std::optional opt_other(opt_value); // // optional<int>
    }

    SECTION("smart pointers")
    {
        std::unique_ptr<int> up{new int(13)}; // CTAD disabled for raw pointers as arguments
        std::shared_ptr<int> sp{new int(13)}; // CTAD disabled for raw pointers as arguments

        std::shared_ptr sp2 = std::move(up); // CTAD - std::shared_ptr<int>
        std::weak_ptr wp = sp; // std::weak_ptr<int>
    }

    SECTION("std::function")
    {
        std::function f = foo; // std::function<int(int, int)>

        std::function l = [](const std::string& w) { std::cout << w << "\n"; }; // std::function<void(const std::string&)>
    }

    SECTION("containers")
    {
        std::vector vec = {1, 2, 3, 4};
        std::list lst = {1, 4, 5, 6};

        std::vector data1(begin(lst), end(lst)); // std::vector<int>
        std::vector data2{begin(lst), end(lst)}; // std::vector<std::list<int>::iterator>>

        std::array table = {1, 2, 3, 4, 5};
    }
}

///////////////////////////////////

template <typename T, size_t N>
struct Array
{
    T items[N];
};

template <typename THead, typename... TTail>
Array(THead, TTail...) -> Array<THead, sizeof...(TTail) + 1>;

TEST_CASE("interesting deduction guide")
{
    Array arr = {1.0, 2, 3, 4, 5}; // Array<int, 5>
}