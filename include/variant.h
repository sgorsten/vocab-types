#ifndef EARLY17_VARIANT
#define EARLY17_VARIANT

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include "utility.h"

namespace std {

// Forward declare types
template<class... Types> class variant;

////////////////////////////////////////////////////////////////////////////
// monostate - http://en.cppreference.com/w/cpp/utility/variant/monostate //
////////////////////////////////////////////////////////////////////////////

struct monostate {};
constexpr bool operator<(monostate, monostate) noexcept { return false; }
constexpr bool operator>(monostate, monostate) noexcept { return false; }
constexpr bool operator<=(monostate, monostate) noexcept { return true; }
constexpr bool operator>=(monostate, monostate) noexcept { return true; }
constexpr bool operator==(monostate, monostate) noexcept { return true; }
constexpr bool operator!=(monostate, monostate) noexcept { return false; }	

//////////////////////////////////////////////////////////////////////////////////////////////
// bad_variant_access - http://en.cppreference.com/w/cpp/utility/variant/bad_variant_access //
//////////////////////////////////////////////////////////////////////////////////////////////

class bad_variant_access : public std::exception { public: bad_variant_access() : std::exception() {} const char * what() const noexcept override { return "bad_variant_access"; } };

//////////////////////////////////////////////////////////////////////////////////
// variant_size - http://en.cppreference.com/w/cpp/utility/variant/variant_size //
//////////////////////////////////////////////////////////////////////////////////

template<class T> struct variant_size;
template<class... Types> struct variant_size<std::variant<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {}; // (2)
template<class T> class variant_size<const T> : public variant_size<T> {}; // (3)
template<class T> class variant_size<volatile T> : public variant_size<T> {}; // (3)
template<class T> class variant_size<const volatile T> : public variant_size<T> {}; // (3)	

////////////////////////////////////////////////////////////////////////////////////////////////
// variant_alternative - http://en.cppreference.com/w/cpp/utility/variant/variant_alternative //
////////////////////////////////////////////////////////////////////////////////////////////////

template<size_t I, class T> class variant_alternative;
template<class First, class... Rest> class variant_alternative<0, variant<First, Rest...>> { public: typedef First type; };
template<size_t I, class First, class... Rest> class variant_alternative<I, variant<First, Rest...>> { public: typedef typename variant_alternative<I-1, variant<Rest...>>::type type; };
template<size_t I, class T> class variant_alternative<I, const T> { public: typedef std::add_const_t<typename variant_alternative<I,T>::type> type; };
template<size_t I, class T> class variant_alternative<I, volatile T> { public: typedef std::add_volatile_t<typename variant_alternative<I,T>::type> type; };
template<size_t I, class T> class variant_alternative<I, const volatile T> { public: typedef std::add_cv_t<typename variant_alternative<I,T>::type> type; };
template<size_t I, class T> using variant_alternative_t = typename variant_alternative<I, T>::type;

//////////////////////////////////////////////////////////////////////////////////
// variant_npos - http://en.cppreference.com/w/cpp/utility/variant/variant_npos //
//////////////////////////////////////////////////////////////////////////////////

constexpr std::size_t variant_npos = -1;

// Forward declare std::get<I> so that visit(...) can work properly
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> & get(variant<Types...> & v);
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> && get(variant<Types...> && v);
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> const & get(const variant<Types...> & v);
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> const && get(const variant<Types...> && v);

namespace detail {

// Determine which constructor would be selected based on a type
template<class... Types> struct constructor_selection_helper {};
template<> struct constructor_selection_helper<> { void f(); };
template<class First, class... Rest> struct constructor_selection_helper<First, Rest...> : constructor_selection_helper<Rest...> { First f(First); using constructor_selection_helper<Rest...>::f; };
template<class... Types, class T> auto construct(T && t) { return constructor_selection_helper<Types...>{}.f(std::forward<T>(t)); }

// Determine the index of a type in a type list
template<class T, class... Types> struct index_of;
template<class T, class... Rest> struct index_of<T, T, Rest...> { constexpr static size_t value = 0; };
template<class T, class First, class... Rest> struct index_of<T, First, Rest...> { constexpr static size_t value = 1 + index_of<T, Rest...>::value; };

template<class Visitor, class Variant> auto visit(Variant && var, index_t<0>, Visitor && vis) { return var.index() == 0 ? vis(std::get<0>(var)) : throw std::bad_variant_access{}; }
template<class Visitor, class Variant, size_t I> auto visit(Variant && var, index_t<I>, Visitor && vis) { return var.index() == I ? vis(std::get<I>(var)) : visit(std::forward<Variant>(var), index_t<I-1>{}, std::forward<Visitor>(vis)); }

template<class Visitor, class Left, class Right> auto visit_same(Left && l, Right && r, index_t<0>, Visitor && vis) { return r.index() == 0 ? vis(std::get<0>(std::forward<Left>(l)), std::get<0>(std::forward<Right>(r))) : throw std::bad_variant_access{}; }
template<class Visitor, class Left, class Right, size_t I> auto visit_same(Left && l, Right && r, index_t<I>, Visitor && vis) { return r.index() == I ? vis(std::get<I>(std::forward<Left>(l)), std::get<I>(std::forward<Right>(r))) : visit_same(std::forward<Left>(l), std::forward<Right>(r), index_t<I-1>{}, std::forward<Visitor>(vis)); }
template<class Visitor, class Left, class Right> auto visit_same(Left && l, Right && r, Visitor && vis) { return visit_same(std::forward<Left>(l), std::forward<Right>(r), index_t<variant_size<std::remove_reference_t<Right>>::value-1>{}, std::forward<Visitor>(vis)); }
template<class T> void invoke_destructor(T & x) { x.~T(); }

template<class Variant> void swap_contents(Variant & lhs, Variant & rhs) { using std::swap; visit_same(lhs, rhs, [](auto & l, auto & r) { return swap(l, r); }); }

} // namespace std::detail

////////////////////////////////////////////////////////////////
// variant - http://en.cppreference.com/w/cpp/utility/variant //
////////////////////////////////////////////////////////////////

template<class... Types> class variant
{
public:
    //////////////////////////////////////////////////////////////////////////////
    // (constructor) - http://en.cppreference.com/w/cpp/utility/variant/variant //
    //////////////////////////////////////////////////////////////////////////////

    constexpr variant()                                                                                                         { emplace<0>(); } // (1)
    variant(const variant & other)                                                                                              { if(!other.valueless_by_exception()) _Construct(other); } // (2)
    variant(variant && other)                                                                                                   { if(!other.valueless_by_exception()) _Construct(std::move(other)); } // (3)
    template<class T> variant(T && t)                                                                                           { *this = std::forward<T>(t); } // (4)

    template<class T, class... Args> explicit variant(in_place_type_t<T>, Args&&... args)                                       { emplace<T>(std::forward<Args>(args)...); } // (5)
    template<class T, class U, class... Args > explicit variant(in_place_type_t<T>, initializer_list<U> il, Args&&... args)     { emplace<T>(il, std::forward<Args>(args)...); } // (6)
    template<size_t I, class... Args> explicit variant(in_place_index_t<I>, Args&&... args)                                     { emplace<I>(std::forward<Args>(args)...); } // (7)
    template<size_t I, class U, class... Args> explicit variant(in_place_index_t<I>, initializer_list<U> il, Args&&... args)    { emplace<I>(il, std::forward<Args>(args)...); } // (8)

    ////////////////////////////////////////////////////////////////////////////////
    // (destructor) - http://en.cppreference.com/w/cpp/utility/variant/%7Evariant //
    ////////////////////////////////////////////////////////////////////////////////

    ~variant() { _Reset(); }

    ///////////////////////////////////////////////////////////////////////////////
    // operator = - http://en.cppreference.com/w/cpp/utility/variant/operator%3D //
    ///////////////////////////////////////////////////////////////////////////////

    variant& operator=(const variant& rhs) // (1)
    {
        if(rhs._Index == variant_npos) _Reset();
        else if(_Index == rhs._Index) _Assign(rhs);
        else *this = variant{rhs};
        return *this;
    }

    variant& operator=(variant&& rhs) // (2)
    {
        if(rhs._Index == variant_npos) _Reset();
        else if(_Index == rhs._Index) _Assign(std::move(rhs));
        else _Construct(std::move(rhs));
        return *this;
    }

    template<class T> std::enable_if_t<!std::is_same<std::remove_reference_t<std::remove_cv_t<T>>, variant>::value, variant> & operator=(T && t) // (3)
    {
        using U = decltype(detail::construct<Types...>(std::forward<T>(t)));
        constexpr size_t I = detail::index_of<U, Types...>::value;
        if(_Index == I) reinterpret_cast<variant_alternative_t<I, variant<Types...>> &>(_Storage) = std::forward<T>(t);
        else emplace<I>(std::forward<T>(t));
        return *this;
    }

    ////////////////////////////////////////////////////////////////////
    // index - http://en.cppreference.com/w/cpp/utility/variant/index //
    ////////////////////////////////////////////////////////////////////

    constexpr size_t index() const noexcept { return _Index; }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // valueless_by_exception - http://en.cppreference.com/w/cpp/utility/variant/valueless_by_exception //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    constexpr bool valueless_by_exception() const noexcept { return _Index == variant_npos; }

    /////////////////////////////////////////////////////////////////////////
    // emplace - http://en.cppreference.com/w/cpp/utility/variant/emplace/ //
    /////////////////////////////////////////////////////////////////////////

    template<class T, class... Args> void emplace(Args&&... args) { emplace<detail::index_of<T, Types...>::value>(std::forward<Args>(args)...); }                                                                       // (1)
    template<class T, class U, class... Args> void emplace(std::initializer_list<U> il, Args&&... args) { emplace<detail::index_of<T, Types...>::value>(il, std::forward<Args>(args)...); }                             // (2)
    template<size_t I, class... Args> void emplace(Args&&... args) { _Reset(); new(&_Storage) variant_alternative_t<I, variant>(std::forward<Args>(args)...); _Index = I; }                                             // (3)
    template<size_t I, class U, class... Args> void emplace(std::initializer_list<U> il, Args&&... args) { _Reset(); new(&_Storage) variant_alternative_t<I, variant>(il, std::forward<Args>(args)...); _Index = I; }   // (4)

    //////////////////////////////////////////////////////////////////
    // swap - http://en.cppreference.com/w/cpp/utility/variant/swap //
    //////////////////////////////////////////////////////////////////

    void swap(variant& rhs) // (1)
    {
        if(_Index == rhs._Index)
        {
            if(_Index == variant_npos) return;
            detail::swap_contents(*this, rhs);
        }
        else
        {
            variant tmp {std::move(*this)};
            *this = std::move(rhs);
            rhs = std::move(tmp);
        }
    }

    //template<size_t I> variant_alternative_t<I, variant> & _Unchecked_get() noexcept { return reinterpret_cast<variant_alternative_t<I, variant> &>(_Storage); }
    //template<size_t I> variant_alternative_t<I, variant> const & _Unchecked_get() const noexcept { return reinterpret_cast<variant_alternative_t<I, variant> const &>(_Storage); }
//private:
    void _Reset()
    {
        if(_Index != variant_npos)
        {
            visit([](auto & x) { detail::invoke_destructor(x); }, *this);
            _Index = variant_npos;
        }
    }

    template<class U> void _Construct(U && rhs) try
    { 
        _Reset();
        _Index = rhs._Index;
        detail::visit_same(*this, std::forward<U>(rhs), [](auto & l, auto && r) { new(&l) std::remove_reference_t<decltype(l)>(std::forward<decltype(r)>(r)); }); 
    }
    catch(...)
    {
        _Index = variant_npos;
        throw;
    }

    template<class U> void _Assign(U && rhs)
    {
        assert(_Index == rhs._Index);
        detail::visit_same(*this, std::forward<U>(rhs), [](auto & l, auto && r) { l = std::forward<decltype(r)>(r); });
    }

    std::aligned_union_t<0, Types...> _Storage;
    size_t _Index = variant_npos;
};

//////////////////////////////////////////////////////////////
// visit - http://en.cppreference.com/w/cpp/utility/variant //
//////////////////////////////////////////////////////////////

template<class Visitor> auto visit(Visitor && vis) { return vis(); }
template<class Visitor, class First, class... Rest> auto visit(Visitor && vis, First && first, Rest &&... rest)
{ 
    return detail::visit(first, detail::index_t<std::variant_size<std::remove_reference_t<First>>::value-1>{}, [&](auto && x) 
    { 
        return visit([&](auto &&... args) 
        { 
            return vis(x, args...); 
        }, rest...);
    });
}

////////////////////////////////////////////////////////////////////////////////////////////
// holds_alternative - http://en.cppreference.com/w/cpp/utility/variant/holds_alternative //
////////////////////////////////////////////////////////////////////////////////////////////

template<class T, class... Types> constexpr bool holds_alternative(const variant<Types...> & v) { return detail::index_of<T, Types...>::value == v.index(); }

////////////////////////////////////////////////////////////////
// get - http://en.cppreference.com/w/cpp/utility/variant/get //
////////////////////////////////////////////////////////////////

template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> & get(variant<Types...> & v) { if(v.index() == I) return reinterpret_cast<variant_alternative_t<I, variant<Types...>> &>(v._Storage); throw bad_variant_access{}; }
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> && get(variant<Types...> && v) { return std::move(get<I>(v)); }                        
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> const & get(const variant<Types...> & v) { if(v.index() == I) return reinterpret_cast<variant_alternative_t<I, variant<Types...>> const &>(v._Storage); throw bad_variant_access{}; }
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> const && get(const variant<Types...> && v) { return std::move(get<I>(v)); }
template<class T, class... Types> constexpr       T &  get(      variant<Types...> &  v) { return get<detail::index_of<T, Types...>::value>(v); }
template<class T, class... Types> constexpr       T && get(      variant<Types...> && v) { return get<detail::index_of<T, Types...>::value>(std::move(v)); }
template<class T, class... Types> constexpr const T &  get(const variant<Types...> &  v) { return get<detail::index_of<T, Types...>::value>(v); }
template<class T, class... Types> constexpr const T && get(const variant<Types...> && v) { return get<detail::index_of<T, Types...>::value>(std::move(v)); }

//////////////////////////////////////////////////////////////////////
// get_if - http://en.cppreference.com/w/cpp/utility/variant/get_if //
//////////////////////////////////////////////////////////////////////

template<std::size_t I, class... Types> constexpr std::add_pointer_t<std::variant_alternative_t<I, std::variant<Types...>>> get_if(std::variant<Types...>* pv) noexcept { return pv && pv->index() == I ? &std::get<I>(*pv) : nullptr; }
template<std::size_t I, class... Types> constexpr std::add_pointer_t<const std::variant_alternative_t<I, variant<Types...>>> get_if(const std::variant<Types...>* pv) noexcept { return pv && pv->index() == I ? &std::get<I>(*pv) : nullptr; }
template<class T, class... Types> constexpr std::add_pointer_t<T> get_if(variant<Types...>* pv) noexcept { return get_if<detail::index_of<T, Types...>::value>(pv); }
template<class T, class... Types> constexpr std::add_pointer_t<const T> get_if(const variant<Types...>* pv) noexcept { return get_if<detail::index_of<T, Types...>::value>(pv); }

//////////////////////////////////////////////////////////////////////////////////////////////////
// operator==, !=, <, <=, >, >= - http://en.cppreference.com/w/cpp/utility/variant/operator_cmp //
//////////////////////////////////////////////////////////////////////////////////////////////////

template<class... Types> constexpr bool operator==(const variant<Types...>& v, const variant<Types...>& w) // (1)
{ 
    return v.index() != w.index() ? false 
        : v.valueless_by_exception() ? true 
        : detail::visit_same(v, w, [](const auto & a, const auto & b) { return a == b; });
}
template<class... Types> constexpr bool operator!=(const variant<Types...>& v, const variant<Types...>& w) // (2)
{ 
    return v.index() != w.index() ? true 
        : v.valueless_by_exception() ? false 
        : detail::visit_same(v, w, [](const auto & a, const auto & b) { return a != b; }); 
} 
template<class... Types> constexpr bool operator<(const variant<Types...>& v, const variant<Types...>& w) // (3)
{ 
    return w.valueless_by_exception() ? false
        : v.valueless_by_exception() ? true
        : v.index() < w.index() ? true
        : v.index() > w.index() ? false
        : detail::visit_same(v, w, [](const auto & a, const auto & b) { return a < b; });
}
template<class... Types> constexpr bool operator>(const variant<Types...>& v, const variant<Types...>& w) // (4)
{ 
    return v.valueless_by_exception() ? false
        : w.valueless_by_exception() ? true
        : v.index() > w.index() ? true
        : v.index() < w.index() ? false
        : detail::visit_same(v, w, [](const auto & a, const auto & b) { return a > b; });
}
template<class... Types> constexpr bool operator<=(const variant<Types...>& v, const variant<Types...>& w) // (5)
{ 
    return v.valueless_by_exception() ? true
        : w.valueless_by_exception() ? false
        : v.index() < w.index() ? true
        : v.index() > w.index() ? false
        : detail::visit_same(v, w, [](const auto & a, const auto & b) { return a <= b; });
}
template<class... Types> constexpr bool operator>=(const variant<Types...>& v, const variant<Types...>& w) // (5)
{ 
    return w.valueless_by_exception() ? true
        : v.valueless_by_exception() ? false
        : v.index() > w.index() ? true
        : v.index() < w.index() ? false
        : detail::visit_same(v, w, [](const auto & a, const auto & b) { return a >= b; });
}
	
////////////////////////////////////////////////////////////////////////
// std::swap - http://en.cppreference.com/w/cpp/utility/variant/swap2 //
////////////////////////////////////////////////////////////////////////

template<class... Types> void swap(variant<Types...>& lhs, variant<Types...>& rhs) { lhs.swap(rhs); }

///////////////////////////////////////////////////////////////////////////
// hash<variant> - http://en.cppreference.com/w/cpp/utility/variant/hash //
///////////////////////////////////////////////////////////////////////////

template<class... Types> struct hash<std::variant<Types...>>
{
    size_t operator() (const std::variant<Types...> & key) const
    {
        return key.valueless_by_exception() ? 0 : key.index() ^ std::visit([](const auto & value) noexcept { return std::hash<std::remove_const_t<std::remove_reference_t<decltype(value)>>>{}(value); }, key);
    }
};

} // namespace std

#endif
