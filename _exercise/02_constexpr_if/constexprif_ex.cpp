#include <iostream>
#include <list>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Explain
{
    template <typename T>
    struct Identity
    {
        using type  = T;
    };

    template <typename T>
    using Identity_t = typename Identity<T>::type;

    ///////////////////
    
    template <typename T1, typename T2>
    struct IsSame
    {
        static constexpr bool value = false;
    };

    template <typename T>
    struct IsSame<T, T>
    {
        static constexpr bool value = true;
    };

    template <typename T1, typename T2>
    static constexpr bool IsSame_v = IsSame<T1, T2>::value;
}

TEST_CASE("type traits & metafunction")
{
    using namespace Explain;

    Identity<int>::type var1;
    Identity_t<int> var2;

    std::vector<int> vec;

    using vec_iterator = std::vector<int>::iterator;
    using vec_iterator_tag = std::iterator_traits<vec_iterator>::iterator_category;
}


template <typename Iterator>
void advance_it(Iterator& it, size_t n)
{
    using it_category = typename std::iterator_traits<Iterator>::iterator_category;

    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, it_category>)
    {
        it += n; // random_access_iterator || contiguous_iterator_tag (C++20)
    } 
    else 
    {
        while(n--)
        {
            ++it;
        }
    }
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // random_access_iterator

        auto it = data.begin();

        std::advance(it, 3); // it += n

        REQUIRE(*it == 4);
    }

    SECTION("input_iterator")
    {
        list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // bidirectional_iterator

        auto it = data.begin();

        advance_it(it, 3); // n-times ++it

        REQUIRE(*it == 4);
    }
}