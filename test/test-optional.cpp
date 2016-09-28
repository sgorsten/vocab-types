#include "optional.h"
#include "doctest.h"
#include <string>

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
