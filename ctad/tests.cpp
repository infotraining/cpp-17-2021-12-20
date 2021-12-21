#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

void foo(int) {}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("Template Argument Deduction - case 1")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    auto a1 = x; // int
    deduce1(x);
    auto a2 = cx; // int
    deduce1(cx);
    auto a3 = ref_x; // int
    deduce1(ref_x);
    auto a4 = cref_x; // int
    deduce1(cref_x);
    auto a5 = tab; // int* - decay to pointer
    deduce1(tab);
    auto a6 = foo; // void(*)(int)
    deduce1(foo);
}

TEST_CASE("Template Argument Deduction - case 2")
{
    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10];

    auto& a1 = x; // int&
    deduce2(x);
    auto& a2 = cx; // const int&
    deduce2(cx);
    auto& a3 = ref_x; // int&
    deduce2(ref_x);
    auto& a4 = cref_x; // const int&
    deduce2(cref_x);
    auto& a5 = tab; // int(&)[10]
    deduce2(tab);
    auto& a6 = foo; // void(&)(int)
    deduce2(foo);
}

TEST_CASE("Template Argument Deduction - case 3")
{
    std::string str = "text";

    auto&& a1 = str; // std::string&
    deduce3(str);

    auto&& a2 = std::string("text"); // std::string&&
    deduce3(std::string("text"));
}`

TEST_CASE("auto - special case")
{
    auto il = {1, 2, 3}; // std::initializer_list
    //deduce1({1, 2, 3}); // ERROR
}