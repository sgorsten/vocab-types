#include "string_view.h"
#include "doctest.h"

TEST_CASE("std::string_view")
{
    std::string_view a;
    std::string_view b{"Hello world!", 12};
    std::string_view c{"Hello world!"};
}

TEST_CASE("find(...), etc.")
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

TEST_CASE("find_first_of(...), etc.")
{
    std::string_view a {"This is a test"};
    CHECK(a.find_first_of("tsa") == 3); // finds 's'
    CHECK(a.find_first_of("ta") == 8); // finds 'a'
    CHECK(a.find_first_of("t") == 10); // finds 't'

    CHECK(a.find_first_not_of("This ") == 8); // finds 'a'
    CHECK(a.find_first_not_of("This a") == 10); // finds 't'
    CHECK(a.find_first_not_of("ihT") == 3); // finds 's'

    CHECK(a.find_last_of("tsa") == 13); // finds 't'
    CHECK(a.find_last_of("sa") == 12); // finds 's'
    CHECK(a.find_last_of("a") == 8); // finds 'a'

    CHECK(a.find_last_not_of("test ") == 8); // finds 'a'
    CHECK(a.find_last_not_of("test a") == 5); // finds 'i'
    CHECK(a.find_last_not_of("tis ae") == 1); // finds 'h'
}