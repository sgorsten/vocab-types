#ifndef EARLY17_OPTIONAL
#define EARLY17_OPTIONAL

#include "variant.h"

namespace std {

/////////////////////////////////////////////////////////////////////////
// nullopt - http://en.cppreference.com/w/cpp/utility/optional/nullopt //
/////////////////////////////////////////////////////////////////////////

struct nullopt_t { constexpr nullopt_t(int) {} };
constexpr nullopt_t nullopt {0};

/////////////////////////////////////////////////////////////////////////////////////////////////
// bad_optional_access - http://en.cppreference.com/w/cpp/utility/optional/bad_optional_access //
/////////////////////////////////////////////////////////////////////////////////////////////////

class bad_optional_access : public std::exception { public: bad_optional_access() : std::exception("bad_optional_access") {} };

template<class T> class optional
{
public:
    variant<monostate, T> _Value;

    ////////////////////////////////////////////////////////////////////////////////
    // (constructor) - http://en.cppreference.com/w/cpp/utility/optional/optional //
    ////////////////////////////////////////////////////////////////////////////////

    constexpr optional() = default; // (1)
    constexpr optional(nullopt_t) {} // (1)
    optional(const optional & other) = default; // (2)
    optional(optional && other) = default; // (3)
    constexpr optional(const T & value) : _Value(value) {} // (4)
    constexpr optional(T && value) : _Value(move(value)) {} // (5)
    template<class... Args> constexpr explicit optional(in_place_t, Args&&... args) : _Value(in_place<1>, std::forward<Args>(args)...) {} // (6)
    template<class U, class... Args> constexpr explicit optional(in_place_t, initializer_list<U> ilist, Args&&... args) : _Value(in_place<1>, ilist, std::forward<Args>(args)...) {} // (7)

    //////////////////////////////////////////////////////////////////////////////////
    // (destructor) - http://en.cppreference.com/w/cpp/utility/optional/%7Eoptional //
    //////////////////////////////////////////////////////////////////////////////////

    ~optional() = default;

    ///////////////////////////////////////////////////////////////////////////////
    // operator= - http://en.cppreference.com/w/cpp/utility/optional/operator%3D //
    ///////////////////////////////////////////////////////////////////////////////

    optional& operator= (std::nullopt_t) { _Value = monostate{}; return *this; } // (1)
    optional& operator= (const optional& other) = default; // (2)
    optional& operator= (optional&& other) = default; // (3)
    template<class U> optional& operator=(U&& value) { _Value = std::forward<U>(value); return *this; } // (4)

    ////////////////////////////////////////////////////////////////////////////////
    // operator->,* - http://en.cppreference.com/w/cpp/utility/optional/operator* //
    ////////////////////////////////////////////////////////////////////////////////

    constexpr const T* operator->() const { return reinterpret_cast<const T *>(&_Value._Storage); } // (1)
    T* operator->() { return reinterpret_cast<T *>(&_Value._Storage); } // (1)
    constexpr const T& operator*() const& { return reinterpret_cast<const T &>(_Value._Storage); } // (2)
    T& operator*() & { return reinterpret_cast<T &>(_Value._Storage); } // (2)
    constexpr const T&& operator*() const&& { return reinterpret_cast<const T &&>(_Value._Storage); } // (2)
    T&& operator*() && { return reinterpret_cast<T &&>(_Value._Storage); } // (2)

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // operator bool, has_value - http://en.cppreference.com/w/cpp/utility/optional/operator_bool //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    constexpr explicit operator bool() const { return _Value.index() == 1; }
    constexpr bool has_value() const { return _Value.index() == 1; }

    /////////////////////////////////////////////////////////////////////
    // value - http://en.cppreference.com/w/cpp/utility/optional/value //
    /////////////////////////////////////////////////////////////////////

    T& value() & { if(!has_value()) throw bad_optional_access{}; return std::get<1>(_Value); } // (1)
    constexpr const T & value() const & { if(!has_value()) throw bad_optional_access{}; return std::get<1>(_Value); } // (1)
    T&& value() && { if(!has_value()) throw bad_optional_access{}; return std::get<1>(std::move(_Value)); } // (2)
    constexpr const T&& value() const && { if(!has_value()) throw bad_optional_access{}; return std::get<1>(std::move(_Value)); } // (2)

    ///////////////////////////////////////////////////////////////////////////
    // value_or - http://en.cppreference.com/w/cpp/utility/optional/value_or //
    ///////////////////////////////////////////////////////////////////////////

    template<class U> constexpr T value_or(U && default_value ) const & { return has_value() ? value() : default_value; }
    template<class U> T value_or(U && default_value ) && { return has_value() ? std::move(value()) : default_value; }

    ///////////////////////////////////////////////////////////////////
    // swap - http://en.cppreference.com/w/cpp/utility/optional/swap //
    ///////////////////////////////////////////////////////////////////

    void swap(optional & other) { _Value.swap(other._Value); }

    /////////////////////////////////////////////////////////////////////
    // reset - http://en.cppreference.com/w/cpp/utility/optional/reset //
    /////////////////////////////////////////////////////////////////////

    void reset() { _Value = monostate{}; }

    /////////////////////////////////////////////////////////////////////////
    // emplace - http://en.cppreference.com/w/cpp/utility/optional/emplace //
    /////////////////////////////////////////////////////////////////////////

    template<class... Args> void emplace(Args &&... args) { _Value.emplace(std::in_place_index_t<1>{}, std::forward<Args>(args)...); }
    template<class U, class... Args> void emplace(initializer_list<U> ilist, Args&&... args) { _Value.emplace(std::in_place_index_t<1>{}, ilist, std::forward<Args>(args)...); }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// operator==, !=, <, <=, >, >= - http://en.cppreference.com/w/cpp/utility/optional/operator_cmp //
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> constexpr bool operator==(const optional<T> & lhs, const optional<T> & rhs) { return lhs._Value == rhs._Value; } // (1)
template<class T> constexpr bool operator!=(const optional<T> & lhs, const optional<T> & rhs) { return lhs._Value != rhs._Value; } // (2)
template<class T> constexpr bool operator< (const optional<T> & lhs, const optional<T> & rhs) { return lhs._Value <  rhs._Value; } // (3)
template<class T> constexpr bool operator<=(const optional<T> & lhs, const optional<T> & rhs) { return lhs._Value <= rhs._Value; } // (4)
template<class T> constexpr bool operator> (const optional<T> & lhs, const optional<T> & rhs) { return lhs._Value >  rhs._Value; } // (5)
template<class T> constexpr bool operator>=(const optional<T> & lhs, const optional<T> & rhs) { return lhs._Value >= rhs._Value; } // (6)
	
template<class T> constexpr bool operator==(const optional<T>& opt, std::nullopt_t) { return opt == optional<T>{nullopt}; }  // (7)
template<class T> constexpr bool operator==(std::nullopt_t, const optional<T>& opt) { return optional<T>{nullopt} == opt; } // (8)
template<class T> constexpr bool operator!=(const optional<T>& opt, std::nullopt_t) { return opt != optional<T>{nullopt}; } // (9)
template<class T> constexpr bool operator!=(std::nullopt_t, const optional<T>& opt) { return optional<T>{nullopt} != opt; } // (10)
template<class T> constexpr bool operator< (const optional<T>& opt, std::nullopt_t) { return opt <  optional<T>{nullopt}; } // (11)
template<class T> constexpr bool operator< (std::nullopt_t, const optional<T>& opt) { return optional<T>{nullopt} <  opt; } // (12)
template<class T> constexpr bool operator<=(const optional<T>& opt, std::nullopt_t) { return opt <= optional<T>{nullopt}; } // (13)
template<class T> constexpr bool operator<=(std::nullopt_t, const optional<T>& opt) { return optional<T>{nullopt} <= opt; } // (14)
template<class T> constexpr bool operator> (const optional<T>& opt, std::nullopt_t) { return opt >  optional<T>{nullopt}; } // (15)
template<class T> constexpr bool operator> (std::nullopt_t, const optional<T>& opt) { return optional<T>{nullopt} >  opt; } // (16)
template<class T> constexpr bool operator>=(const optional<T>& opt, std::nullopt_t) { return opt >= optional<T>{nullopt}; } // (17)
template<class T> constexpr bool operator>=(std::nullopt_t, const optional<T>& opt) { return optional<T>{nullopt} >= opt; } // (18)

template<class T> constexpr bool operator==(const optional<T>& opt, const T& value) { return opt == optional<T>{value}; } // (19)
template<class T> constexpr bool operator==(const T& value, const optional<T>& opt) { return optional<T>{value} == opt; } // (20)
template<class T> constexpr bool operator!=(const optional<T>& opt, const T& value) { return opt != optional<T>{value}; } // (21)
template<class T> constexpr bool operator!=(const T& value, const optional<T>& opt) { return optional<T>{value} != opt; } // (22)
template<class T> constexpr bool operator< (const optional<T>& opt, const T& value) { return opt <  optional<T>{value}; } // (23)
template<class T> constexpr bool operator< (const T& value, const optional<T>& opt) { return optional<T>{value} <  opt; } // (24)
template<class T> constexpr bool operator<=(const optional<T>& opt, const T& value) { return opt <= optional<T>{value}; } // (25)
template<class T> constexpr bool operator<=(const T& value, const optional<T>& opt) { return optional<T>{value} <= opt; } // (26)
template<class T> constexpr bool operator> (const optional<T>& opt, const T& value) { return opt >  optional<T>{value}; } // (27)
template<class T> constexpr bool operator> (const T& value, const optional<T>& opt) { return optional<T>{value} >  opt; } // (28)
template<class T> constexpr bool operator>=(const optional<T>& opt, const T& value) { return opt >= optional<T>{value}; } // (29)
template<class T> constexpr bool operator>=(const T& value, const optional<T>& opt) { return optional<T>{value} >= opt; } // (30)

/////////////////////////////////////////////////////////////////////////////////////
// make_optional - http://en.cppreference.com/w/cpp/utility/optional/make_optional //
/////////////////////////////////////////////////////////////////////////////////////

template<class T> constexpr std::optional<std::decay_t<T>> make_optional( T&& value ) { return std::optional<std::decay_t<T>>(std::forward<T>(value)); } // (1)
template<class T, class... Args> constexpr std::optional<T> make_optional( Args&&... args ) { return std::optional<T>(std::in_place, std::forward<Args>(args)...); } // (2)
template<class T, class U, class... Args> constexpr std::optional<T> make_optional( std::initializer_list<U> il, Args&&... args ) { return std::optional<T>(std::in_place, il, std::forward<Args>(args)...); } // (3)

////////////////////////////////////////////////////////////////////
// swap - http://en.cppreference.com/w/cpp/utility/optional/swap2 //
////////////////////////////////////////////////////////////////////

template<class T> void swap(optional<T>& lhs, optional<T>& rhs) noexcept(lhs.swap(rhs)) { lhs.swap(rhs); }

/////////////////////////////////////////////////////////////////////////////
// hash<optional> - http://en.cppreference.com/w/cpp/utility/optional/hash //
/////////////////////////////////////////////////////////////////////////////

template<class T> struct hash<std::optional<T>>
{
    size_t operator() (const std::optional<T> & key) noexcept { return key ? std::hash<T>{}(*key) : 0; }
};

} // namespace std

#endif
