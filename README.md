[![Build status](https://ci.appveyor.com/api/projects/status/1ylttmu6a95uhsoy?svg=true)](https://ci.appveyor.com/project/sgorsten/vocab-types)

# vocab-types

This is a doomed project. Its days were numbered before it even began.

It is an independent implementation of some of the new vocabulary types which are being introduced in the C++17 standard library. While these types are already available on some compilers within the `std::experimental` namespace, they are not presently available in official capacity within Visual Studio. I feel that these types are important enough in their ability to clarify the semantics of C++ code that they are worth adopting early. To that end, I will maintain this repository, striving (but probably not succeeding) to produce a standards-compliant implementation of these types, until such time as they become available by default.

Note that as these files are intended to be temporary standins for the actual standard library headers, they define their types in `namespace std`, which is normally reserved for the standard library implementation.

# Contents

- [string_view.h](/include/string_view.h): A standin for the `<string_view>` standard library header
- [optional.h](/include/optional.h): A standin for the `<optional>` standard library header
- [variant.h](/include/variant.h): A standin for the `<variant>` standard library header
- [any.h](/include/any.h): A standin for the `<any>` standard library header
- [utility.h](/include/utility.h): Contains the additions to the `<utility>` standard library header required by `std::optional<T>`, `std::variant<T...>`, and `std::any`

# Known Gaps

- [ ] std::hash (variant / string_view)
- [ ] forwarding allocators (variant)
- [ ] find_first_of/etc. (string_view)
- [ ] noexcept specifications are missing/incomplete
- [ ] constexpr specifications are missing/incomplete
- [ ] SFINAE for disabling some overloads based on type traits
- [ ] Most of the API needs to be tested (are there existing unit tests for this?)

# License

The files listed above are 100% my original work, based solely on the documentation available at [cppreference.com](http://en.cppreference.com/w/), which I believe allows me to offer them under the terms of the [Unlicense](http://unlicense.org/). 

The unit tests are built with Viktor Kirilov's [doctest](https://github.com/onqtam/doctest) framework, which is available under the terms of the [MIT Software License](https://opensource.org/licenses/MIT).
