Platform | Build Status |
-------- | ------------ |
| Visual Studio 2015 | [Appveyor](http://appveyor.com): [![Build status](https://ci.appveyor.com/api/projects/status/1ylttmu6a95uhsoy?svg=true)](https://ci.appveyor.com/project/sgorsten/vocab-types) |
| GCC 4.9 and Clang 3.7 | [Travis CI](http://travis-ci.org): [![Build status](http://travis-ci.org/sgorsten/vocab-types.svg?branch=master)](https://travis-ci.org/sgorsten/vocab-types) |

# vocab-types

This project provides an independent implementation of the functionality provided by the <variant>, <optional>, <any>, and <string_view> C++17 standard library headers. It is intended to provide the full, standards-compliant functionality of these headers, but compile cleanly on the major existing C++14 compilers, namely Microsoft Visual Studio 2015, GCC 6.0, and Clang 3.7. I personally feel that the functionality of the vocabulary types provided by these headers is so valuable to code clarity, that they are worth adopting early. For that reason, I will maintain this repository, adding missing features, fixing bugs, and improving compliance with the C++17 standard, until such time as these headers become available in an official capacity.

Note that, as these files are intended to be a valid implementation of the actual standard library headers, they define their types in `namespace std`.

# Contents

- [string_view.h](/include/string_view.h): A standin for the `<string_view>` standard library header
- [optional.h](/include/optional.h): A standin for the `<optional>` standard library header
- [variant.h](/include/variant.h): A standin for the `<variant>` standard library header
- [any.h](/include/any.h): A standin for the `<any>` standard library header
- [utility.h](/include/utility.h): Contains the additions to the `<utility>` standard library header required by `std::optional<T>`, `std::variant<T...>`, and `std::any`

# Known Gaps

- [ ] forwarding allocators (variant)
- [ ] noexcept specifications are missing/incomplete
- [ ] constexpr specifications are missing/incomplete
- [ ] SFINAE for disabling some overloads based on type traits
- [ ] Most of the API needs to be tested (are there existing unit tests for this?)

# License

The files listed above are 100% my original work, based solely on the documentation available at [cppreference.com](http://en.cppreference.com/w/), which I believe allows me to offer them under the terms of the [Unlicense](http://unlicense.org/). 

The unit tests are built with Viktor Kirilov's [doctest](https://github.com/onqtam/doctest) framework, which is available under the terms of the [MIT Software License](https://opensource.org/licenses/MIT).
