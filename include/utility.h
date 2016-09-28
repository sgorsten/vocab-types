#ifndef EARLY17_UTILITY
#define EARLY17_UTILITY

namespace std {

namespace detail {

template<class T> struct tag_t {};
template<size_t I> struct index_t {};

} // namespace std::detail

//////////////////////////////////////////////////////////////////
// in_place - http://en.cppreference.com/w/cpp/utility/in_place //
//////////////////////////////////////////////////////////////////

struct in_place_tag { in_place_tag() = delete; }; 
inline std::in_place_tag in_place() { throw std::logic_error{"bad call"}; } // (1)
template<class T> std::in_place_tag in_place(detail::tag_t<T>) { throw std::logic_error{"bad call"}; } // (2)
template<std::size_t I> std::in_place_tag in_place(detail::index_t<I>) { throw std::logic_error{"bad call"}; } // (3)
using in_place_t = std::in_place_tag (&)();
template<class T> using in_place_type_t = std::in_place_tag (&)(detail::tag_t<T>);
template<std::size_t I> using in_place_index_t = std::in_place_tag (&)(detail::index_t<I>);

} // namespace std

#endif
