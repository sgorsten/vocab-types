[![License is Unlicense](http://img.shields.io/badge/license-Unlicense-blue.svg?style=flat)](http://unlicense.org/)

Platform | Build Status |
-------- | ------------ |
| Visual Studio 2015 | [Appveyor](http://appveyor.com): [![Build status](https://ci.appveyor.com/api/projects/status/1ylttmu6a95uhsoy?svg=true)](https://ci.appveyor.com/project/sgorsten/vocab-types) |
| GCC 6.0 and Clang 3.7 | [Travis CI](http://travis-ci.org): [![Build status](http://travis-ci.org/sgorsten/vocab-types.svg?branch=master)](https://travis-ci.org/sgorsten/vocab-types) |

# vocab-types

This project provides an independent implementation of the functionality provided by the <variant>, <optional>, <any>, and <string_view> C++17 standard library headers. It is intended to provide the full, standards-compliant functionality of these headers, but compile cleanly on the major existing C++14 compilers, namely Microsoft Visual Studio 2015, GCC 6.0, and Clang 3.7. I personally feel that the functionality of the vocabulary types provided by these headers is so valuable to code clarity, that they are worth adopting early. For that reason, I will maintain this repository, adding missing features, fixing bugs, and improving compliance with the C++17 standard, until such time as these headers become available in an official capacity.

Note that, as these files are intended to be a valid implementation of the actual standard library headers, they define their types in `namespace std`. The intention is that you can start using them in your code exactly as you would an official implementation, and when an official implementation becomes available, simply delete these files from your source tree to transition over.

# Known Gaps

- Allocator forwarding constructors (variant) have not been implemented
- noexcept specifications are incomplete
- constexpr specifications are incomplete
- SFINAE for disabling some overloads based on type traits has not been implemented
- Most of the API needs to be tested (are there existing unit tests for this?)
- In a standards compliant implementation, `std::basic_string` is responsible for conversion to/from `std::basic_string_view`. Since this library only introduces `std::basic_string_view` and does not modify the definition of any existing standard library classes, we have to reverse this responsibility and add it to `std::basic_string_view` instead. This may lead to some subtly different behavior in some cases.

# License

The files listed above are 100% my original work, based solely on the documentation available at [cppreference.com](http://en.cppreference.com/w/), which I believe allows me to offer them under the terms of the [Unlicense](http://unlicense.org/). 

The unit tests are built with Viktor Kirilov's [doctest](https://github.com/onqtam/doctest) framework, which is available under the terms of the [MIT Software License](https://opensource.org/licenses/MIT).
