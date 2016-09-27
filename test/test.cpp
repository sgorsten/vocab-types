#include "optional.h"
#include "variant.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <string>
#include <vector>
#include <type_traits>

TEST_CASE("std::variant<T...> traits")
{
    typedef std::variant<bool, float> bool_float_variant;
    typedef std::variant<int, double, std::string> int_double_string_variant;

    CHECK(std::variant_size<bool_float_variant>::value == 2);
    CHECK(std::variant_size<const bool_float_variant>::value == 2);
    CHECK(std::variant_size<volatile bool_float_variant>::value == 2);
    CHECK(std::variant_size<const volatile bool_float_variant>::value == 2);

    CHECK(std::variant_size<int_double_string_variant>::value == 3);
    CHECK(std::variant_size<const int_double_string_variant>::value == 3);
    CHECK(std::variant_size<volatile int_double_string_variant>::value == 3);
    CHECK(std::variant_size<const volatile int_double_string_variant>::value == 3);

    CHECK(typeid(std::variant_alternative_t<0, std::variant<bool, float>> *) == typeid(bool *));
    CHECK(typeid(std::variant_alternative_t<1, std::variant<bool, float>> *) == typeid(float *));
    CHECK(typeid(std::variant_alternative_t<0, const std::variant<bool, float>> *) == typeid(const bool *));
    CHECK(typeid(std::variant_alternative_t<1, const std::variant<bool, float>> *) == typeid(const float *));
    CHECK(typeid(std::variant_alternative_t<0, volatile std::variant<bool, float>> *) == typeid(volatile bool *));
    CHECK(typeid(std::variant_alternative_t<1, volatile std::variant<bool, float>> *) == typeid(volatile float *));
    CHECK(typeid(std::variant_alternative_t<0, const volatile std::variant<bool, float>> *) == typeid(const volatile bool *));
    CHECK(typeid(std::variant_alternative_t<1, const volatile std::variant<bool, float>> *) == typeid(const volatile float *));

    CHECK(typeid(std::variant_alternative_t<0, std::variant<int, double, std::string>> *) == typeid(int *));
    CHECK(typeid(std::variant_alternative_t<1, std::variant<int, double, std::string>> *) == typeid(double *));
    CHECK(typeid(std::variant_alternative_t<2, std::variant<int, double, std::string>> *) == typeid(std::string *));
    CHECK(typeid(std::variant_alternative_t<0, const std::variant<int, double, std::string>> *) == typeid(const int *));
    CHECK(typeid(std::variant_alternative_t<1, const std::variant<int, double, std::string>> *) == typeid(const double *));
    CHECK(typeid(std::variant_alternative_t<2, const std::variant<int, double, std::string>> *) == typeid(const std::string *));
    CHECK(typeid(std::variant_alternative_t<0, volatile std::variant<int, double, std::string>> *) == typeid(volatile int *));
    CHECK(typeid(std::variant_alternative_t<1, volatile std::variant<int, double, std::string>> *) == typeid(volatile double *));
    CHECK(typeid(std::variant_alternative_t<2, volatile std::variant<int, double, std::string>> *) == typeid(volatile std::string *));
    CHECK(typeid(std::variant_alternative_t<0, const volatile std::variant<int, double, std::string>> *) == typeid(const volatile int *));
    CHECK(typeid(std::variant_alternative_t<1, const volatile std::variant<int, double, std::string>> *) == typeid(const volatile double *));
    CHECK(typeid(std::variant_alternative_t<2, const volatile std::variant<int, double, std::string>> *) == typeid(const volatile std::string *));
}

TEST_CASE("construct std::variant<T...>")
{
    std::variant<int, double, std::string> a;
    std::variant<int, double, std::string> b {55};
    std::variant<int, double, std::string> c {3.14};
    std::variant<int, double, std::string> d {"Hello world!"};

    CHECK(!a.valueless_by_exception());
    CHECK(!b.valueless_by_exception());
    CHECK(!c.valueless_by_exception());
    CHECK(!d.valueless_by_exception());

    CHECK(a.index() == 0);
    CHECK(b.index() == 0);
    CHECK(c.index() == 1);
    CHECK(d.index() == 2);

    CHECK(std::get<0>(a) == int{0});
    CHECK(std::get<0>(b) == int{55});
    CHECK(std::get<1>(c) == double{3.14});
    CHECK(std::get<2>(d) == std::string{"Hello world!"});

    b = d;
    CHECK(!b.valueless_by_exception());
    CHECK(b.index() == 2);
    CHECK(std::get<2>(b) == std::string{"Hello world!"});

    d = c;
    CHECK(!d.valueless_by_exception());
    CHECK(d.index() == 1);
    CHECK(std::get<1>(d) == double{3.14});
}

TEST_CASE("variant move and copy construction and assignment")
{
    typedef std::variant<int, std::vector<double>> variant_t;

    // Construct an int alternative
    variant_t a {5};
    CHECK(a.index() == 0);
    CHECK(std::get<0>(a) == 5);

    // Construct a std::vector<double> alternative
    variant_t b {std::vector<double>{1,2,3,4,5,6,7,8,9,10}};
    CHECK(b.index() == 1);
    CHECK(std::get<1>(b).size() == 10);

    // Copy construct from b, which should leave b unchanged
    variant_t c {b};
    CHECK(c.index() == 1);
    CHECK(std::get<1>(c).size() == 10);
    CHECK(b.index() == 1);
    CHECK(std::get<1>(b).size() == 10);

    // Move construct from b, which should leave b representing a moved-from std::vector<double>
    variant_t d {std::move(b)};
    CHECK(d.index() == 1);
    CHECK(std::get<1>(d).size() == 10);
    CHECK(b.index() == 1);
    CHECK(std::get<1>(b).size() == 0);

    // Now copy a into b, which should change b to represent an int
    b = a;
    CHECK(b.index() == 0);
    CHECK(std::get<0>(b) == 5);
    CHECK(a.index() == 0);
    CHECK(std::get<0>(a) == 5);

    // Now move d into a, which should change a to represent a std::vector<double> and leave d as a moved-from std::vector<double>
    a = std::move(d);
    CHECK(a.index() == 1);
    CHECK(std::get<1>(a).size() == 10);
    CHECK(d.index() == 1);
    CHECK(std::get<1>(d).size() == 0);
}

TEST_CASE("variant emplace()")
{
    // Default-construct several variants
    std::variant<int, std::vector<double>> a, b, c, d;
    CHECK(a.index() == 0);
    CHECK(std::get<0>(a) == 0);

    // Emplace the first alternative by index
    a.emplace<0>(55);
    CHECK(a.index() == 0);
    CHECK(std::get<0>(a) == 55);

    // Emplace the second alternative by index
    b.emplace<1>(std::vector<double>{1,2,3,4,5});
    CHECK(b.index() == 1);
    CHECK(std::get<1>(b).size() == 5);

    // Emplace the first alternative by type
    c.emplace<int>(72);
    CHECK(c.index() == 0);
    CHECK(std::get<0>(c) == 72);

    // Emplace the second alternative by type
    d.emplace<std::vector<double>>(std::vector<double>{3,2,1});
    CHECK(d.index() == 1);
    CHECK(std::get<1>(d).size() == 3);
}

TEST_CASE("construct null std::optional<T>")
{
    const std::optional<int> a;
    CHECK(!a);
    CHECK(!a.has_value());
    CHECK(a == std::nullopt);
    CHECK(std::nullopt == a);

    const std::optional<std::string> b {std::nullopt};
    CHECK(!b);
    CHECK(!b.has_value());
    CHECK(b == std::nullopt);
    CHECK(std::nullopt == b);
}

TEST_CASE("construct std::optional<T> with a value")
{
    const std::optional<int> a {55};
    CHECK(a);
    CHECK(a.has_value());
    CHECK(a.value() == 55);
    CHECK(a == 55);
    CHECK(55 == a);

    const std::optional<std::string> b {"Hello world!"};
    CHECK(b);
    CHECK(b.has_value());

    const std::string s {"Hello world!"};
    CHECK(b.value() == s);
    CHECK(b == s);
    CHECK(s == b);
}