#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <array>

#include "catch.hpp"

using namespace std;

TEST_CASE("string_view")
{
    SECTION("creating string_view")
    {
        const char* cstr = "text";

        std::string_view sv1 = cstr;

        REQUIRE(sv1.size() == 4);
        REQUIRE(sv1 == "text"sv);

        std::cout << sv1 << "\n";
        
        std::string str = "string text";

        std::string_view sv2 = str;
        REQUIRE(sv2.size() == 11);

        auto sv3 = "text"sv;
    }
}

TEST_CASE("difference between string & string_view")
{
    std::string_view sv_empty;
    REQUIRE(sv_empty.data() == nullptr);

    std::string str_empty;
    REQUIRE(str_empty.data() != nullptr);

    const char buffer[3] = {'a', 'b', 'c'};

    std::string_view sv(buffer, 3);
    std::cout << sv << "\n";

    REQUIRE(sv == "abc");
}

TEST_CASE("conversions")
{
    SECTION("string -> string_view")
    {
        string s = "abc";
        string_view sv = s;

        string_view sv_dangling = std::string("text"); // BEWARE - dangling pointer
    }

    SECTION("string_view -> string")
    {
        string_view sv = "abc";
        string s(sv);
    }
}

template <typename TContainer>
void print_all(const TContainer& container, std::string_view prefix)
{
    std::cout << prefix << ": [ ";
    for(const auto& item : container)
        cout << item << " ";
    cout << "]\n";
}

TEST_CASE("using string_view")
{
    print_all(vector{1, 2, 3}, "vec");

    std::string name = "data";
    print_all(vector{5, 6, 7}, name);
}

string_view get_prefix(string_view text, size_t length)
{
    assert(text.size() >= length);
    
    return {text.data(), length};
}

TEST_CASE("beware")
{
    REQUIRE(get_prefix("c++17", 3) == "c++"sv);
    REQUIRE(get_prefix("c++17"s, 3) == "c++"sv); // dangling pointer
}

TEST_CASE("string_view as constexpr")
{
    constexpr std::array ids = {"ericsson"sv, "lg"sv, "hp"sv };
}