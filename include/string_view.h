#ifndef EARLY17_STRING_VIEW
#define EARLY17_STRING_VIEW

#include <iterator>
#include <stdexcept>

#pragma warning(push)
#pragma warning(disable : 4814) // "in C++14 'constexpr' will not imply 'const'" - We know and are specifying constexpr non-const functions deliberately

namespace std {

///////////////////////////////////////////////////////////////////////////////////
// basic_string_view - http://en.cppreference.com/w/cpp/string/basic_string_view //
///////////////////////////////////////////////////////////////////////////////////

template<class CharT, class Traits = std::char_traits<CharT>> 
class basic_string_view
{
public:
    using traits_type =	Traits;
    using value_type = CharT;
    using pointer = CharT*;
    using const_pointer = const CharT*;
    using reference = CharT&;
    using const_reference = const CharT&;
    using const_iterator = const_pointer; // Note: implementation-defined RandomAccessIterator;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    const_pointer _Data;
    size_type _Size;

    /////////////////////////////////////////////////////////////////////////////////////////////////
    // (constructor) - http://en.cppreference.com/w/cpp/string/basic_string_view/basic_string_view //
    /////////////////////////////////////////////////////////////////////////////////////////////////

    constexpr basic_string_view() noexcept : _Data{nullptr}, _Size{0} {} // (1)
    constexpr basic_string_view(const basic_string_view& other) noexcept = default; // (2)
    constexpr basic_string_view(const CharT* s, size_type count) noexcept : _Data{s}, _Size{count} {} // (3)
    constexpr basic_string_view(const CharT* s) : _Data{s}, _Size{Traits::length(s)} {} // (4)

    ///////////////////////////////////////////////////////////////////////////////////////
    // operator= - http://en.cppreference.com/w/cpp/string/basic_string_view/operator%3D // 
    ///////////////////////////////////////////////////////////////////////////////////////

    basic_string_view& operator= (const basic_string_view & view) noexcept = default;

    /////////////////////////////////////////////////////////////////////////////
    // begin - http://en.cppreference.com/w/cpp/string/basic_string_view/begin //
    /////////////////////////////////////////////////////////////////////////////

    constexpr const_iterator begin() const noexcept { return cbegin(); }
    constexpr const_iterator cbegin() const noexcept { return _Data; }

    /////////////////////////////////////////////////////////////////////////
    // end - http://en.cppreference.com/w/cpp/string/basic_string_view/end //
    /////////////////////////////////////////////////////////////////////////

    constexpr const_iterator end() const noexcept { return cend(); }
    constexpr const_iterator cend() const noexcept { return _Data + _Size; }

    ///////////////////////////////////////////////////////////////////////////////
    // rbegin - http://en.cppreference.com/w/cpp/string/basic_string_view/rbegin //
    /////////////////////////////////////////////////////////////////////////////

    constexpr const_reverse_iterator rbegin() const noexcept { return cend(); }
    constexpr const_reverse_iterator rcbegin() const noexcept { return cend(); }

    ///////////////////////////////////////////////////////////////////////////
    // rend - http://en.cppreference.com/w/cpp/string/basic_string_view/rend //
    ///////////////////////////////////////////////////////////////////////////

    constexpr const_reverse_iterator rend() const noexcept { return cbegin(); }
    constexpr const_reverse_iterator rcend() const noexcept { return cbegin(); }

    ////////////////////////////////////////////////////////////////////////////////////////
    // operator[] - http://en.cppreference.com/w/cpp/string/basic_string_view/operator_at //
    ////////////////////////////////////////////////////////////////////////////////////////

    constexpr const_reference operator[](size_type pos) const noexcept { return _Data[pos]; }

    ///////////////////////////////////////////////////////////////////////
    // at - http://en.cppreference.com/w/cpp/string/basic_string_view/at //
    ///////////////////////////////////////////////////////////////////////

    constexpr const_reference at(size_type pos) const { if(pos >= _Size) throw std::out_of_range{}; return _Data[pos]; }

    /////////////////////////////////////////////////////////////////////////////
    // front - http://en.cppreference.com/w/cpp/string/basic_string_view/front //
    /////////////////////////////////////////////////////////////////////////////

    constexpr const_reference front() const noexcept { return _Data[0]; }

    ///////////////////////////////////////////////////////////////////////////
    // back - http://en.cppreference.com/w/cpp/string/basic_string_view/back //
    ///////////////////////////////////////////////////////////////////////////

    constexpr const_reference back() const noexcept { return _Data[_Size-1]; }

    ///////////////////////////////////////////////////////////////////////////
    // data - http://en.cppreference.com/w/cpp/string/basic_string_view/data //
    ///////////////////////////////////////////////////////////////////////////

    constexpr const_pointer data() const noexcept { return _Data; }

    ///////////////////////////////////////////////////////////////////////////
    // size - http://en.cppreference.com/w/cpp/string/basic_string_view/size //
    ///////////////////////////////////////////////////////////////////////////

    constexpr size_type size() const noexcept { return _Size; }
    constexpr size_type length() const noexcept { return _Size; }

    ///////////////////////////////////////////////////////////////////////////////////
    // max_size - http://en.cppreference.com/w/cpp/string/basic_string_view/max_size //
    ///////////////////////////////////////////////////////////////////////////////////

    constexpr size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); } // TODO: Is this correct? Do we have to account for what subset of memory is accessible?

    /////////////////////////////////////////////////////////////////////////////
    // empty - http://en.cppreference.com/w/cpp/string/basic_string_view/empty //
    /////////////////////////////////////////////////////////////////////////////

    constexpr bool empty() const noexcept { return _Size == 0; }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // remove_prefix - http://en.cppreference.com/w/cpp/string/basic_string_view/remove_prefix //
    /////////////////////////////////////////////////////////////////////////////////////////////

    constexpr void remove_prefix(size_type n) noexcept { _Data += n; _Size -= n; }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // remove_suffix - http://en.cppreference.com/w/cpp/string/basic_string_view/remove_suffix //
    /////////////////////////////////////////////////////////////////////////////////////////////

    constexpr void remove_suffix(size_type n) noexcept { _Size -= n; }

    ///////////////////////////////////////////////////////////////////////////
    // swap - http://en.cppreference.com/w/cpp/string/basic_string_view/swap //
    ///////////////////////////////////////////////////////////////////////////

    constexpr void swap(basic_string_view& v) noexcept { swap(_Data, v._Data); swap(_Size, v._Size); }

    // TODO: All find variations

    ///////////////////////////////////////////////////////////////////////////
    // npos - http://en.cppreference.com/w/cpp/string/basic_string_view/npos //
    ///////////////////////////////////////////////////////////////////////////

    static constexpr size_type npos = size_type(-1);
};

using string_view =	basic_string_view<char>;
using wstring_view = basic_string_view<wchar_t>;
using u16string_view = basic_string_view<char16_t>;
using u32string_view = basic_string_view<char32_t>;

// TODO: Comparison operators

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<< (std::basic_ostream<CharT, Traits>& os, std::basic_string_view<CharT, Traits> v)
{
    for(auto ch : v) os << ch;
    return os;
}

// TODO: Hash

} // namespace std

#pragma warning(pop) 

#endif
