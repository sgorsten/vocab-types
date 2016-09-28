#include "any.h"
#include "doctest.h"
#include <string>

TEST_CASE("construct std::any")
{
    std::any a;
    std::any b {5};
    std::any c {std::string{"Hello world!"}};

    CHECK(!a.has_value());
    CHECK(a.type() == typeid(void));
    CHECK(std::any_cast<int>(&a) == nullptr);

    CHECK(b.has_value());
    CHECK(b.type() == typeid(int));
    CHECK(std::any_cast<int>(&b) != nullptr);
    CHECK(std::any_cast<int>(b) == 5);

    CHECK(c.has_value());
    CHECK(c.type() == typeid(std::string));
    CHECK(std::any_cast<std::string>(&c) != nullptr);
    CHECK(std::any_cast<std::string>(c) == "Hello world!");

    auto d = std::make_any<double>(3.14);
    CHECK(d.has_value());
    CHECK(d.type() == typeid(double));
    CHECK(std::any_cast<double>(&d) != nullptr);
    CHECK(std::any_cast<double>(d) == 3.14);
}