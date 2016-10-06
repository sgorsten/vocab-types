// any.h is an independent implementation of the C++17 <any> standard
// library header, which can be compiled by C++14 compliant compilers.
// The original author of this file is Sterling Orsten, and its
// permanent home is https://github.com/sgorsten/vocab-types

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

#ifndef EARLY17_ANY
#define EARLY17_ANY

#include "utility.h"
#include <memory>

namespace std {

class any
{
    struct holder_base
    {
        virtual ~holder_base() = default;
        virtual std::unique_ptr<holder_base> clone() = 0;
        virtual const type_info& type() = 0;        
        virtual void * get() = 0;        
    };
    template<class T> struct holder : holder_base
    {
        T value;
        holder(T value) : value{std::move(value)} {}
        std::unique_ptr<holder_base> clone() { return std::unique_ptr<holder_base>(new holder(value)); }
        const type_info& type() { return typeid(T); }
        void * get() { return &value; }
    };
    std::unique_ptr<holder_base> _Value;
public:
    template<class T> T * _Get() noexcept { return _Value && type() == typeid(T) ? reinterpret_cast<T *>(_Value->get()) : nullptr; } 
    template<class T> const T * _Get() const noexcept { return _Value && type() == typeid(T) ? reinterpret_cast<const T *>(_Value->get()) : nullptr; }

    //////////////////////////////////////////////////////////////////////
    // (constructor) - http://en.cppreference.com/w/cpp/utility/any/any //
    //////////////////////////////////////////////////////////////////////

    constexpr any() = default; // (1)
    any(const any& other) : _Value{other._Value ? other._Value->clone() : nullptr} {} // (2)
    any(any&& other) : _Value{move(other._Value)} {} // (3)
    template<class ValueType> any(ValueType&& value) : _Value{new holder<std::decay_t<ValueType>>{std::forward<ValueType>(value)}} {}
    template<class T, class... Args> explicit any(in_place_type_t<T>, Args&&... args) { emplace<T>(std::forward<Args>(args)...); } // (5)
    template<class T, class U, class... Args> explicit any(in_place_type_t<T>, initializer_list<U> il, Args&&... args) { emplace<T>(il, std::forward<Args>(args)...); } // (6)

    ////////////////////////////////////////////////////////////////////////
    // (destructor) - http://en.cppreference.com/w/cpp/utility/any/%7Eany //
    ////////////////////////////////////////////////////////////////////////

    ~any() { reset(); }

    //////////////////////////////////////////////////////////////////////////
    // operator= - http://en.cppreference.com/w/cpp/utility/any/operator%3D //
    //////////////////////////////////////////////////////////////////////////

    any& operator=( const any& rhs ) { return *this = any{rhs}; } // (1)
    any& operator=( any&& rhs ) = default; // (2)
    template<typename ValueType> any& operator=( ValueType&& rhs ) { return *this = any{std::forward<ValueType>(rhs)}; } // (3)

    ////////////////////////////////////////////////////////////////////
    // emplace - http://en.cppreference.com/w/cpp/utility/any/emplace //
    ////////////////////////////////////////////////////////////////////

    template<class T, class... Args> void emplace( Args&&... args ) { _Value = std::unique_ptr<holder_base>{new holder<T>{std::forward<Args>(args)...}}; } // (1)
    template<class T, class U, class... Args> void emplace( std::initializer_list<U> il, Args&&... args ) { _Value = std::unique_ptr<holder_base>{new holder<T>{il, std::forward<Args>(args)...}}; } // (2)

    ////////////////////////////////////////////////////////////////
    // reset - http://en.cppreference.com/w/cpp/utility/any/reset //
    ////////////////////////////////////////////////////////////////

    void reset() noexcept { _Value.reset(); }

    //////////////////////////////////////////////////////////////
    // swap - http://en.cppreference.com/w/cpp/utility/any/swap //
    //////////////////////////////////////////////////////////////

    void swap(any& other) noexcept { _Value.swap(other._Value); }

    ////////////////////////////////////////////////////////////////////////
    // has_value - http://en.cppreference.com/w/cpp/utility/any/has_value //
    ////////////////////////////////////////////////////////////////////////

    bool has_value() const noexcept { return !!_Value; }

    //////////////////////////////////////////////////////////////
    // type - http://en.cppreference.com/w/cpp/utility/any/type //
    //////////////////////////////////////////////////////////////

    const type_info& type() const noexcept { return _Value ? _Value->type() : typeid(void); }
};

////////////////////////////////////////////////////////////////////
// std::swap - http://en.cppreference.com/w/cpp/utility/any/swap2 //
////////////////////////////////////////////////////////////////////

inline void swap(any& lhs, any& rhs) noexcept { lhs.swap(rhs); }

//////////////////////////////////////////////////////////////////////////////
// bad_any_cast - http://en.cppreference.com/w/cpp/utility/any/bad_any_cast //
//////////////////////////////////////////////////////////////////////////////

class bad_any_cast : public bad_cast { public: bad_any_cast() : bad_cast() {} };

//////////////////////////////////////////////////////////////////////
// any_cast - http://en.cppreference.com/w/cpp/utility/any/any_cast //
//////////////////////////////////////////////////////////////////////

template<class ValueType> ValueType any_cast(const any& operand) { if(operand.type() != typeid(ValueType)) throw bad_any_cast{}; return *any_cast<std::add_const_t<std::remove_reference_t<ValueType>>>(&operand); } // (1)
template<class ValueType> ValueType any_cast(any& operand) { if(operand.type() != typeid(ValueType)) throw bad_any_cast{}; return *any_cast<std::remove_reference_t<ValueType>>(&operand); } // (2)
template<class ValueType> ValueType any_cast(any&& operand) { if(operand.type() != typeid(ValueType)) throw bad_any_cast{}; return *any_cast<std::remove_reference_t<ValueType>>(&operand); } // (3)
template<class ValueType> const ValueType* any_cast(const any* operand) noexcept { return operand ? operand->_Get<ValueType>() : nullptr; } // (4)
template<class ValueType> ValueType* any_cast(any* operand) noexcept { return operand ? operand->_Get<ValueType>() : nullptr; } // (5)

//////////////////////////////////////////////////////////////////////
// make_any - http://en.cppreference.com/w/cpp/utility/any/make_any //
//////////////////////////////////////////////////////////////////////

template<class T, class... Args> any make_any(Args&&... args) { return std::any(std::in_place<T>, std::forward<Args>(args)...); } // (1)
template<class T, class U, class... Args> any make_any(std::initializer_list<U> il, Args&&... args) { return std::any(std::in_place<T>, il, std::forward<Args>(args)...); } // (2)

} // namespace std

#endif
