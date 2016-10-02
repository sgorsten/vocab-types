#include "string_view.h"
#include "doctest.h"

TEST_CASE("std::string_view")
{
    std::string_view a;
    std::string_view b{"Hello world!", 12};
    std::string_view c{"Hello world!"};
}
