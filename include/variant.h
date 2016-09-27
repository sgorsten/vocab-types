#ifndef EARLY17_VARIANT
#define EARLY17_VARIANT

#include <stdexcept>
#include <type_traits>

namespace std {

namespace detail {

template<class... T> struct variant_view { void * storage; size_t index; };
template<class... T> struct const_variant_view { const void * storage; size_t index; };
template<class F, class... R> variant_view<R...> rest(variant_view<F,R...> view) { return {view.storage, view.index-1}; }
template<class F, class... R> const_variant_view<R...> rest(const_variant_view<F,R...> view) { return {view.storage, view.index-1}; }

// Destruct a variant value
void destruct(variant_view<>) { throw std::logic_error("bad index"); }
template<class F, class... R> void destruct(variant_view<F,R...> view)
{ 
    if(view.index == 0) reinterpret_cast<F *>(view.storage)->~F();
    else destruct(rest(view));
}

// Move-construct a value at l with the same type as r
void move_construct(void *, variant_view<>) { throw std::logic_error("bad index"); }
template<class F, class... R> void move_construct(void * l, variant_view<F,R...> r)
{
    if(r.index == 0) new(l) F{std::move(*reinterpret_cast<F *>(r.storage))};
    else move_construct(l, rest(r));
}

// Copy-construct a value at l with the same type as r
void copy_construct(void *, const_variant_view<>) { throw std::logic_error("bad index"); }
template<class F, class... R> void copy_construct(void * l, const_variant_view<F,R...> r)
{
    if(r.index == 0) new(l) F{*reinterpret_cast<const F *>(r.storage)};
    else copy_construct(l, rest(r));
}

// Move assign the variant r to the storage at l, assuming l was previously constructed to the same type as r
void move_assign(void *, variant_view<>) { throw std::logic_error("bad index"); }
template<class F, class... R> void move_assign(void * l, variant_view<F,R...> r)
{
    if(r.index == 0) *reinterpret_cast<F *>(l) = std::move(*reinterpret_cast<F *>(r.storage));
    else move_assign(l, rest(r));
}

// Copy assign the variant r to the storage at l, assuming l was previously constructed to the same type as r
void copy_assign(void *, const_variant_view<>) { throw std::logic_error("bad index"); }
template<class F, class... R> void copy_assign(void * l, const_variant_view<F,R...> r)
{
    if(r.index == 0) *reinterpret_cast<F *>(l) = *reinterpret_cast<const F *>(r.storage);
    else copy_assign(l, rest(r));
}

// Copy assign the variant r to the storage at l, assuming l was previously constructed to the same type as r
template<class C> bool compare(const void *, const_variant_view<>, C c) { throw std::logic_error("bad index"); }
template<class C, class F, class... R> bool compare(const void * l, const_variant_view<F,R...> r, C c)
{
    return r.index == 0 ? c(*reinterpret_cast<const F *>(l), *reinterpret_cast<const F *>(r.storage)) : compare(l, rest(r), c);
}

// Determine which constructor would be selected based on a type
template<class... Types> struct constructor_selection_helper {};
template<> struct constructor_selection_helper<> { void f(); };
template<class First, class... Rest> struct constructor_selection_helper<First, Rest...> : constructor_selection_helper<Rest...> { First f(First); using constructor_selection_helper<Rest...>::f; };

// Determine the index of a type in a type list
template<class T, class... Types> struct index_of;
template<class T, class... Rest> struct index_of<T, T, Rest...> { constexpr static size_t value = 0; };
template<class T, class First, class... Rest> struct index_of<T, First, Rest...> { constexpr static size_t value = 1 + index_of<T, Rest...>::value; };

template<class T> struct tag_t {};
template<size_t I> struct index_t {};
template<class Visitor, class Variant> auto visit(Variant && var, index_t<0>, Visitor && vis) { return var.index() == 0 ? vis(std::get<0>(var)) : throw std::bad_variant_access{}; }
template<class Visitor, class Variant, size_t I> auto visit(Variant && var, index_t<I>, Visitor && vis) { return var.index() == I ? vis(std::get<I>(var)) : visit(std::forward<Variant>(var), index_t<I-1>{}, std::forward<Visitor>(vis)); }

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

class bad_variant_access : public std::exception { public: bad_variant_access() : std::exception("bad_variant_access") {} };

//////////////////////////////////////////////////////////////////////////////////
// variant_size - http://en.cppreference.com/w/cpp/utility/variant/variant_size //
//////////////////////////////////////////////////////////////////////////////////

template<class... Types> class variant;
template<class T> struct variant_size; // (1)
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

////////////////////////////////////////////////////////////////
// variant - http://en.cppreference.com/w/cpp/utility/variant //
////////////////////////////////////////////////////////////////

template<class... Types> class variant
{
public:
    std::aligned_union_t<0, Types...> _Storage;
    size_t _Index;

    void _Reset()
    {
        if(_Index != variant_npos)
        {
            destruct(_View());
            _Index = variant_npos;
        }
    }

    detail::variant_view<Types...> _View() { return {&_Storage, _Index}; }
    detail::const_variant_view<Types...> _View() const { return {&_Storage, _Index}; }
    
    //////////////////////////////////////////////////////////////////////////////
    // (constructor) - http://en.cppreference.com/w/cpp/utility/variant/variant //
    //////////////////////////////////////////////////////////////////////////////

    constexpr variant()                                                                     : _Index(variant_npos) { emplace<0>(); }                                                            // (1)
    variant(const variant & other)                                                          : _Index(other._Index) { if(_Index != variant_npos) copy_construct(&_Storage, other._View()); } // (2)
    variant(variant &&      other)                                                          : _Index(other._Index) { if(_Index != variant_npos) move_construct(&_Storage, other._View()); } // (3)
    template<class T> variant(T && t)                                                       : _Index(variant_npos) { *this = std::forward<T>(t); }                                              // (4)
    template<class T,  class... Args> explicit variant(in_place_type_t<T>,  Args&&... args) : _Index(variant_npos) { emplace<T>(std::forward<Args>(args)...); }                                 // (5)
    template<size_t I, class... Args> explicit variant(in_place_index_t<I>, Args&&... args) : _Index(variant_npos) { emplace<I>(std::forward<Args>(args)...); }                                 // (7)

    ////////////////////////////////////////////////////////////////////////////////
    // (destructor) - http://en.cppreference.com/w/cpp/utility/variant/%7Evariant //
    ////////////////////////////////////////////////////////////////////////////////

    ~variant() { _Reset(); }

    ///////////////////////////////////////////////////////////////////////////////
    // operator = - http://en.cppreference.com/w/cpp/utility/variant/operator%3D //
    ///////////////////////////////////////////////////////////////////////////////

    variant& operator=(const variant& rhs) // (1)
    {
        if(rhs.valueless_by_exception()) _Reset();
        else if(_Index == rhs._Index) copy_assign(&_Storage, rhs._View());
        else *this = variant{rhs};
        return *this;
    }

    variant& operator=(variant&& rhs) // (2)
    {
        if(rhs.valueless_by_exception()) _Reset();
        else if(_Index == rhs._Index) move_assign(&_Storage, rhs._View());
        else 
        {
            _Reset();
            move_construct(&_Storage, rhs._View());
            _Index = rhs._Index;
        }
        return *this;
    }

    template<class T> std::enable_if_t<!std::is_same<std::remove_reference_t<std::remove_cv_t<T>>, variant>::value, variant> & operator=(T && t) // (3)
    {
        typedef decltype(detail::constructor_selection_helper<Types...>{}.f(std::forward<T>(t))) U;
        constexpr size_t I = detail::index_of<U, Types...>::value;
        if(_Index == I) *reinterpret_cast<U *>(&_Storage) = std::forward<T>(t);
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

    template<class T,  class... Args> void emplace(Args&&... args) { emplace<index_of<T, Types...>::value>(std::forward<Args>(args)...); }                                    // (1)
    template<size_t I, class... Args> void emplace(Args&&... args) { _Reset(); new(&_Storage) variant_alternative_t<I, variant>(std::forward<Args>(args)...); _Index = I; } // (3)
};

//////////////////////////////////////////////////////////////
// visit - http://en.cppreference.com/w/cpp/utility/variant //
//////////////////////////////////////////////////////////////

template<class Visitor> auto visit(Visitor && vis) { return vis(); }
template<class Visitor, class First, class... Rest> auto visit(Visitor && vis, First && first, Rest &&... rest)
{ 
    detail::visit(first, detail::index_t<std::variant_size<std::remove_reference_t<First>>::value-1>{}, [&](auto && x) 
    { 
        visit([&](auto &&... args) 
        { 
            return std::invoke(vis, x, args...); 
        }, rest...);
    });
}

////////////////////////////////////////////////////////////////////////////////////////////
// holds_alternative - http://en.cppreference.com/w/cpp/utility/variant/holds_alternative //
////////////////////////////////////////////////////////////////////////////////////////////

template<class T, class... Types> constexpr bool holds_alternative(const variant<Types...> & v) { return index_of<T, Types...>::value == v.index(); }

////////////////////////////////////////////////////////////////
// get - http://en.cppreference.com/w/cpp/utility/variant/get //
////////////////////////////////////////////////////////////////

template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> & get(variant<Types...> & v) { if(v.index() != I) throw bad_variant_access{}; return reinterpret_cast<variant_alternative_t<I, variant<Types...>> &>(v._Storage);  }
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> && get(variant<Types...> && v) { return std::move(get<I>(v)); }                        
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> const & get(const variant<Types...> & v) { if(v.index() != I) throw bad_variant_access{}; return reinterpret_cast<variant_alternative_t<I, variant<Types...>> const &>(v._Storage);  }
template<size_t I, class... Types> variant_alternative_t<I, variant<Types...>> const && get(const variant<Types...> && v) { return std::move(get<I>(v)); }
template<class T, class... Types> constexpr       T &  get(      variant<Types...> &  v) { return get<index_of<T, Types...>::value>(v); }
template<class T, class... Types> constexpr       T && get(      variant<Types...> && v) { return get<index_of<T, Types...>::value>(std::move(v)); }
template<class T, class... Types> constexpr const T &  get(const variant<Types...> &  v) { return get<index_of<T, Types...>::value>(v); }
template<class T, class... Types> constexpr const T && get(const variant<Types...> && v) { return get<index_of<T, Types...>::value>(std::move(v)); }

//////////////////////////////////////////////////////////////////////////////////////////////////
// operator==, !=, <, <=, >, >= - http://en.cppreference.com/w/cpp/utility/variant/operator_cmp //
//////////////////////////////////////////////////////////////////////////////////////////////////

template<class... Types> constexpr bool operator==(const variant<Types...>& v, const variant<Types...>& w) // (1)
{ 
    return v.index() != w.index() ? false 
        : v.valueless_by_exception() ? true 
        : detail::compare(&v._Storage, w._View(), [](const auto & a, const auto & b) { return a == b; });
}
template<class... Types> constexpr bool operator!=(const variant<Types...>& v, const variant<Types...>& w) // (2)
{ 
    return v.index() != w.index() ? true 
        : v.valueless_by_exception() ? false 
        : detail::compare(&v._Storage, w._View(), [](const auto & a, const auto & b) { return a != b; }); 
} 
template<class... Types> constexpr bool operator<(const variant<Types...>& v, const variant<Types...>& w) // (3)
{ 
    return w.valueless_by_exception() ? false
        : v.valueless_by_exception() ? true
        : v.index() < w.index() ? true
        : v.index() > w.index() ? false
        : detail::compare(&v._Storage, w._View(), [](const auto & a, const auto & b) { return a < b; });
}
template<class... Types> constexpr bool operator>(const variant<Types...>& v, const variant<Types...>& w) // (4)
{ 
    return v.valueless_by_exception() ? false
        : w.valueless_by_exception() ? true
        : v.index() > w.index() ? true
        : v.index() < w.index() ? false
        : detail::compare(&v._Storage, w._View(), [](const auto & a, const auto & b) { return a > b; });
}
template<class... Types> constexpr bool operator<=(const variant<Types...>& v, const variant<Types...>& w) // (5)
{ 
    return v.valueless_by_exception() ? true
        : w.valueless_by_exception() ? false
        : v.index() < w.index() ? true
        : v.index() > w.index() ? false
        : detail::compare(&v._Storage, w._View(), [](const auto & a, const auto & b) { return a <= b; });
}
template<class... Types> constexpr bool operator>=(const variant<Types...>& v, const variant<Types...>& w) // (5)
{ 
    return w.valueless_by_exception() ? true
        : v.valueless_by_exception() ? false
        : v.index() > w.index() ? true
        : v.index() < w.index() ? false
        : detail::compare(&v._Storage, w._View(), [](const auto & a, const auto & b) { return a >= b; });
}
	
} // namespace std

#endif
