// utility.h is an independent implementation of the additions made to the
// <string_view> standard library header in C++17, which can be compiled by 
// C++14 compliant compilers. The original author of this file is Sterling Orsten, 
// and its permanent home is https://github.com/sgorsten/vocab-types

// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>

#ifndef EARLY17_UTILITY
#define EARLY17_UTILITY

#include <cstddef>

namespace std {

namespace detail {

template<class T> struct tag_t {};
template<size_t I> struct index_t {};

} // namespace std::detail

//////////////////////////////////////////////////////////////////
// in_place - http://en.cppreference.com/w/cpp/utility/in_place //
//////////////////////////////////////////////////////////////////

struct in_place_tag { in_place_tag() = delete; }; 
inline in_place_tag in_place() { throw 0; } // (1)
template<class T> in_place_tag in_place(detail::tag_t<T>) { throw 0; } // (2)
template<size_t I> in_place_tag in_place(detail::index_t<I>) { throw 0; } // (3)
using in_place_t = in_place_tag (&)();
template<class T> using in_place_type_t = in_place_tag (&)(detail::tag_t<T>);
template<size_t I> using in_place_index_t = in_place_tag (&)(detail::index_t<I>);

} // namespace std

#endif
