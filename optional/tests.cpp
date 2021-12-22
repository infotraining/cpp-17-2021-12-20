#include <algorithm>
#include <any>
#include <atomic>
#include <charconv>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <variant>
#include <vector>

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
        std::optional<int> opt {42};

        SECTION("unsafe - fast")
        {
            REQUIRE(*opt == 42);

            // opt.reset();
            opt = std::nullopt;

            //*opt = 42; // Beware - UB
        }

        SECTION("safe - slower")
        {
            std::optional opt {665};

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
        std::optional<std::atomic<int>> opt_atomic {std::in_place, 42};
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
    std::optional<bool> flag {false};

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

    if (auto [pos_end, error_code] = std::from_chars(start, end, value); error_code != std::errc {} || pos_end != end)
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

TEST_CASE("optional ref")
{
    // std::optional<int&> opt_ref; // ERROR
    std::optional<std::reference_wrapper<int>> opt_ref;
}

/////////////////////////////////////////////////////
// any

TEST_CASE("std::any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 42;
    anything = 3.14;
    anything = "text"s;
    anything = std::vector {1, 2, 3};

    REQUIRE(std::any_cast<std::vector<int>>(anything) == std::vector {1, 2, 3});
    REQUIRE_THROWS_AS(std::any_cast<double>(anything), std::bad_any_cast);

    std::vector<int>* ptr_vec = std::any_cast<std::vector<int>>(&anything);
    if (ptr_vec)
    {
        for (const auto& item : *ptr_vec)
            std::cout << item << " ";
        std::cout << "\n";
    }
}

///////////////////////////////////////////////////
// variant

TEST_CASE("std::variant")
{
    std::variant<int, double, std::string> v1;

    REQUIRE(std::holds_alternative<int>(v1) == true);
    REQUIRE(std::get<int>(v1) == 0);

    v1 = 3.14;
    v1 = 42;
    v1 = "text"s;

    REQUIRE(std::holds_alternative<std::string>(v1) == true);
    REQUIRE(std::get<std::string>(v1) == "text"s);
    REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);
    REQUIRE(std::get_if<int>(&v1) == nullptr);

    REQUIRE(v1.index() == 2);

    v1.emplace<std::string>(3, 'a');
    REQUIRE(std::get<std::string>(v1) == "aaa"s);

    std::variant<int, int, double> v2;
    v2.emplace<1>(42);
}

struct S
{
    S(int value)
        : value {value}
    {
    }

    int value;
};

TEST_CASE("monostate")
{
    std::variant<std::monostate, int, double, std::string> v1;

    REQUIRE(std::holds_alternative<std::monostate>(v1));
}

class Printer
{
public:
    void operator()(int v) const { std::cout << "int: " << v << "\n"; }
    void operator()(double v) const { std::cout << "double: " << v << "\n"; }
    void operator()(const std::string& v) const { std::cout << "string " << v << "\n"; }
    void operator()(const std::vector<int>& v) const
    {
        std::cout << "vector: ";
        for (const auto& item : v)
        {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }
};

struct A
{
    void operator()(int a) { std::cout << "A::op()(int)\n"; }
};

struct B
{
    void operator()(double a) { std::cout << "B::op()(double)\n"; }
};

///////////////////////////////////////////////////////////////
template <typename... TClosures>
struct overload : TClosures...
{
    using TClosures::operator()...;
};

// deduction guide
template <typename... TClosures>
overload(TClosures...) -> overload<TClosures...>;
///////////////////////////////////////////////////////////////

TEST_CASE("visiting variants")
{
    std::variant<int, double, std::string, std::vector<int>> v = 3.14;
    v = std::vector{1, 2, 3};

    std::visit(Printer{}, v);

    auto printer = overload {
        [](int v) { std::cout << "int: " << v << "\n"; },
        [](double v) { std::cout << "double: " << v << "\n"; },
        [](const std::string& v) { std::cout << "string " << v << "\n"; },
        [](const auto& v) { std::cout << "other object\n"; }
    };

    std::visit(printer, v);
}

[[nodiscard]] std::variant<std::string, std::errc> load_content(const std::string& filename)
{
    if (filename == ""s)
        return std::errc::bad_file_descriptor;
    
    return "content"s;
}

TEST_CASE("using variant")
{
    auto result = load_content("");

    std::visit(overload{ 
        [](const std::string& s) { std::cout << s << "\n"; },
        [](std::errc ec) { std::cout << "Error!!! " <<  static_cast<int>(ec) << std::endl; }
    }, result);
}