#include <optional>
#include "doctest.h"
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

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

TEST_CASE("optional can be used as key type in ordered containers")
{
    std::map<std::optional<int>, double> a {{5, 1.1}, {std::nullopt, 2.3}, {2, 3.5}};
    std::set<std::optional<std::string>> b {std::string{"Hello"}, std::nullopt, std::string{"world!"}};
    std::multimap<std::optional<bool>, float> c {{true, 1.1f}, {false, 2.3f}, {std::nullopt, 3.5f}, {false, 4.8f}};
    std::multiset<std::optional<float>> d {1.1f, 2.3f, std::nullopt, 2.3f, std::nullopt, 4.8f};
}

TEST_CASE("optional can be used as key type in unordered containers")
{
    std::unordered_map<std::optional<int>, double> a {{5, 1.1}, {std::nullopt, 2.3}, {2, 3.5}};
    std::unordered_set<std::optional<std::string>> b {std::string{"Hello"}, std::nullopt, std::string{"world!"}};
    std::unordered_multimap<std::optional<bool>, float> c {{true, 1.1f}, {false, 2.3f}, {std::nullopt, 3.5f}, {false, 4.8f}};
    std::unordered_multiset<std::optional<float>> d {1.1f, 2.3f, std::nullopt, 2.3f, std::nullopt, 4.8f};
}