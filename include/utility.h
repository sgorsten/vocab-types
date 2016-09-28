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
