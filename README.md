# vocab-types

This is a doomed project. Its days were numbered before it even began.

It is an independent implementation of some of the new vocabulary types which are being introduced in the C++17 standard library. While these types are already available on some compilers within the `std::experimental` namespace, they are not presently available in official capacity within Visual Studio. I feel that these types are important enough in their ability to clarify the semantics of C++ code that they are worth adopting early. To that end, I will maintain this repository, striving (but probably not succeeding) to produce a standards-compliant implementation of these types, until such time as they become available by default.

Note that as these files are intended to be temporary standins for the actual standard library headers, they define their types in `namespace std`, which is normally reserved for the standard library implementation.

# Contents

- [optional.h](/include/optional.h): A standin for the `<optional>` standard library header
- [variant.h](/include/variant.h): A standin for the `<variant>` standard library header
