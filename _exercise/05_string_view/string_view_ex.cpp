#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "catch.hpp"

using namespace std;

auto split_text(string_view text, string_view delimiters = " ,"sv)
{
    vector<string_view> words;

    while (true)
    {
        auto cnt = text.find_first_of(delimiters);

        if (cnt == string_view::npos)
        {
            words.push_back(text);
            break;
        }

        words.push_back(string_view(text.data(), cnt));

        text.remove_prefix(++cnt);
    }
    return words;
}

namespace Alternative
{
    std::vector<std::string_view> split_text(string_view text, std::string_view delimiters = " ,"sv)
    {
        std::vector<std::string_view> sv_v;
        
        size_t start = 0;

        while (start < text.size())
        {
            auto pos = text.find_first_of(delimiters, start);

            if (start != pos)
                sv_v.emplace_back(text.substr(start, pos - start));

            if (pos == std::string_view::npos)
                break;

            start = pos + 1;
        }

        return sv_v;
    }
}

TEST_CASE("split with spaces")
{
    string text = "one two three four";

    std::vector<string_view> words = split_text(text);   // [ (&text[0], 3), (&text[5], 3), ...]

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}

TEST_CASE("split with commas")
{
    string text = "one,two,three,four";

    auto words = split_text(text);

    auto expected = {"one", "two", "three", "four"};

    REQUIRE(equal(begin(expected), end(expected), begin(words)));
}
