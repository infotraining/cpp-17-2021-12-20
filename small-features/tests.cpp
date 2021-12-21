#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

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
        for (const auto& v : values)
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
    Aggregate1 agg1 {1, 3.14, "text", {1, 2, 3}};
    agg1.print();

    Aggregate1 agg2 {1};
    agg2.print();

    Aggregate1 agg3 {};
    agg3.print();

    Cpp17::Aggregate2 agg4 {{1, 3.14, "text"}, {1, 2, 3, 4}};
    agg4.print();
    REQUIRE(agg4.data == std::vector {1, 2, 3, 4});

    Cpp17::Data ds1 {{"dataset"}, {1, 2, 3, 4}};

    Cpp17::WTF wtf {};
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

enum Coffee
{
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

    static_assert(sizeof(int) >= 4);
}

/////////////////////////////////////////////////////////////////////////////////////////
// lambda expressions

namespace LambdaExpressions
{
    struct Gadget
    {
        string id;
        int usage_counter = 0;

        explicit Gadget(string id)
            : id {move(id)}
        {
            cout << "Gadget(" << this->id << " - " << this << ")\n";
        }

        Gadget(const Gadget& source)
            : id {source.id}
        {
            cout << "Gadget(cc: " << this->id << " - " << this << ")\n";
        }

        Gadget(Gadget&& source) noexcept
            : id {move(source.id)}
        {
            cout << "Gadget(mv: " << this->id << " - " << this << ")\n";
        }

        ~Gadget()
        {
            cout << "~Gadget(" << this->id << " - " << this << ")\n";
        }

        void use()
        {
            ++usage_counter;

            auto printer = [this]() { std::cout << "Using " << id << "\n"; };
            printer();
        }

        auto get_reporter()
        {
            //return [self = *this] { std::cout << "Report from Gadget(" << self.id << ")\n"; }; // since C++14
            return [*this]() mutable { id += "#"; std::cout << "Report from Gadget(" << id << ")\n"; }; // since C++17
        }
    };
}

TEST_CASE("lambda expressions")
{
    LambdaExpressions::Gadget g {"ipad"};
    g.use();

    std::function<void()> reporter;

    {
        LambdaExpressions::Gadget smartwatch{"smartwatch"};
        reporter = smartwatch.get_reporter();
    } // destruction of smartwatch

    std::cout << "------\n";

    reporter();
    reporter();
    reporter();
}

constexpr int cube(int x)
{
    return x * x * x;
}

template <size_t N>
constexpr auto create_array()
{
    auto square = [](int x) { return x * x; };

    std::array<int, square(N)> data = { square(1), square(5) };

    for(size_t i = 2; i < square(N); ++i)
        data[i] = square(i);
    
    return data;
}

namespace Cpp20
{
    template <typename Iterator, typename Predicate>
    constexpr Iterator find_if(Iterator first, Iterator last, Predicate pred)
    {
        while(first != last)
        {
            if (pred(*first))
            {
                return first;
            }
            ++first;
        }

        return last;
    }
}

TEST_CASE("constexpr lambda expressions")
{
    auto square = [](int x) { return x * x; };

    int data[square(8)] = {};

    static_assert(std::size(data) == 64);

    constexpr auto lookup_table = create_array<8>();

    constexpr int gt_1000 = *Cpp20::find_if(begin(lookup_table), end(lookup_table), [](int x) { return x > 1000; });
}

/////////////////////////////////////////////
// enum class init

enum Something : uint8_t
{
    some, thing
};

enum class DayOfWeek : uint16_t
{
    mon, tue, wed, thd, fri, sat, sun
};

TEST_CASE("enum vs. enum class")
{
    Something sth{0};
    uint8_t value = sth;

    DayOfWeek day{4};
    std::underlying_type_t<DayOfWeek> day_value = static_cast<std::underlying_type_t<DayOfWeek>>(DayOfWeek::thd);
}

TEST_CASE("bytes in C++")
{
    char old_school_byte1 = 0b00001111;
    char old_school_byte2 = 0b11110000;

    char result1 = ~(old_school_byte1 | old_school_byte2) >> 4;

    char result2 = old_school_byte1 + old_school_byte1;

    std::cout << result1 << "\n";

    std::byte b1{0b1111'0000};
    std::byte b2{0b0000'1111};

    bool flag = true;

    std::byte result = ~(b1 | b2);

    std::cout << std::to_integer<uint32_t>(result) << "\n";
}

/////////////////////////////////////////////////////////////
// auto + {}

TEST_CASE("auto")
{
    int x1 = 42;
    int x2(42);
    int x3{42};
    int x4 = {42};

    auto a1 = 42; // int
    auto a2(42); // int
    //auto a3{42}; // std::initializer_list - C++11/14
    auto a3{42}; // int - change in C++17
    auto a4 = {42}; // std::initializer_list - C++17
    auto a5 = {42, 43}; // std::initializer_list - C++17
}

////////////////////////////////////////////////////////////
// noexcept

void foo(int) {}
void bar(int) noexcept {}


TEST_CASE("noexcept")
{
    //static_assert(std::is_same_v<decltype(foo), decltype(bar)>);

    void (*ptr_fun)(int) noexcept = bar;
}
