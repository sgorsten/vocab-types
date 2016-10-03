#include "string_view.h"
#include "doctest.h"

TEST_CASE("std::string_view")
{
    std::string_view a;
    std::string_view b{"Hello world!", 12};
    std::string_view c{"Hello world!"};
}

TEST_CASE("std::string_view::find(...)")
{
    std::string_view a {"Hello world!"}, b {"o w"}, c {"ow!"}, d {"o"};
    CHECK(a.find(b) == 4);
    CHECK(a.find(c) == std::string_view::npos);
    CHECK(a.find(d) == 4);
    CHECK(a.find(d,5) == 7);

    CHECK(a.rfind(b) == 4);
    CHECK(a.rfind(c) == std::string_view::npos);
    CHECK(a.rfind(d) == 7);
    CHECK(a.rfind(d,7) == 4);
}
