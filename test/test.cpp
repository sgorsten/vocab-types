#include "optional.h"
#include "variant.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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