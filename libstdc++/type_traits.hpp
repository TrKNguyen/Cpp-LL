#include <cstddef> 
#include <utility> 

namespace career {
// ===================================================================
// HELPER CLASSES
// ===================================================================

template<typename T, T v> 
struct integral_constant {
    static constexpr T value = v; 
    using value_type = T; 
    using type = integral_constant<T, v>;

    constexpr operator value_type() const noexcept {return value;}
    constexpr value_type operator()() const noexcept {return value;}
}; 

template<bool B> 
using bool_constant = integral_constant<bool, B>; 

using true_type = integral_constant<bool, true>; 
using false_type = integral_constant<bool, false>;

// ===================================================================
// PRIMARY TYPE CATEGORIS
// =================================================================== 

// is_void 
template<typename T> struct is_void : false_type {}; 
template<> struct is_void<void> : true_type {}; 
template<> struct is_void<const void> : true_type {}; 
template<> struct is_void<volatile void> : true_type {}; 
template<> struct is_void<const volatile void> : true_type{}; 

// is_integral 
template<typename T> struct is_integral: false_type {}; 
template<> struct is_integral<bool> : true_type {};
template<> struct is_integral<char> : true_type {}; 
template<> struct is_integral<signed char> : true_type { };
template<> struct is_integral<unsigned char> : true_type { };
template<> struct is_integral<wchar_t> : true_type { };
template<> struct is_integral<char16_t> : true_type { };
template<> struct is_integral<char32_t> : true_type { };
template<> struct is_integral<short> : true_type { };
template<> struct is_integral<unsigned short> : true_type { };
template<> struct is_integral<int> : true_type { };
template<> struct is_integral<unsigned int> : true_type { };
template<> struct is_integral<long> : true_type { };
template<> struct is_integral<unsigned long> : true_type { };
template<> struct is_integral<long long> : true_type { };
template<> struct is_integral<unsigned long long> : true_type { };

// is_floating_point 
template<typename T> struct is_floating_point: false_type{}; 
template<> struct is_floating_point<float>: true_type{};
template<> struct is_floating_point<double>: true_type{}; 
template<> struct is_floating_point<long double>: true_type{}; 

// is_array 
template<typename T> struct is_array: false_type{};
template<typename T> struct is_array<T[]>: true_type{};
template<typename T, size_t N> struct is_array<T[N]>: true_type{};

// is_pointer 
template<typename T> struct is_pointer: false_type{}; 
template<typename T> struct is_pointer<T*>: true_type{};
template<typename T> struct is_pointer<T* const> : true_type {}; 
template<typename T> struct is_pointer<T* volatile> : true_type {}; 
template<typename T> struct is_pointer<T* const volatile> : true_type{}; 

// is_lvalue_reference
template<typename T> struct is_lvalue_reference: false_type{}; 
template<typename T> struct is_lvalue_reference<T&>: true_type{}; 

// is_rvalue_reference
template<typename T> struct is_rvalue_reference: false_type{}; 
template<typename T> struct is_rvalue_reference<T&&>: true_type{}; 

// ===================================================================
// COMPOSITE TYPE CATEGORIES
// =================================================================== 

// is_reference 
template<typename T> 
struct is_reference : bool_constant<
    is_lvalue_reference<T>::value || is_rvalue_reference<T>::value> {}; 

// is_arithmetic 
template<typename T> 
struct is_arithmetic : bool_constant<
    is_integral<T>::value || is_floating_point<T>::value> {}; 

// ===================================================================
// TYPE PROPERTIES
// ===================================================================

// is_const 
template<typename T> struct is_constant: false_type {}; 
template<typename T> struct is_constant<const T>: true_type{}; 

// is_volatile
template<typename T> struct is_volatile: false_type {}; 
template<typename T> struct is_volatile<volatile T>: true_type{}; 

// ===================================================================
// TYPE RELATIONSHIPS
// ===================================================================

// is_same 
template<typename T, typename U> struct is_same: false_type{}; 
template<typename T> struct is_same<T, T>: true_type{}; 

// ===================================================================
// Const-Volatility Specifiers 
// ===================================================================

// remove_const
template<typename T> 
struct remove_const {
    using type = T;
}; 

template<typename T> 
struct remove_const<const T> {
    using type = T; 
}; 

// remove_volatile
template<typename T> 
struct remove_volatile {
    using type = T;
}; 

template<typename T> 
struct remove_volatile<volatile T> {
    using type = T; 
}; 

// remove_cv
template<typename T> 
struct remove_cv {
    using type = typename remove_volatile<typename remove_const<T>::type>::type;
}; 

// add_const 
template<typename T> 
struct add_const {
    using type = const T;
};

// add_volatile
template<typename T> 
struct add_volatile {
    using type = volatile T;
};

// add_cv
template<typename T> 
struct add_cv {
    using type = const volatile T;
};

// 
// REFERENCES 
// 

// remove_reference
template<typename T> 
struct remove_reference {
    using type = T; 
}; 

template<typename T> 
struct remove_reference<T&> {
    using type = T; 
}; 

template<typename T> 
struct remove_reference<T&&> {
    using type = T; 
}; 

// add_lvalue_reference
namespace detail {
    template<typename T, typename = void> 
    struct add_lvalue_reference_impl {
        using type = T;
    }; 
    template<typename T>
    struct add_lvalue_reference_impl<T, decltype(void(std::declval<T&>()))> {
        using type = T&;
    };
}

template<typename T> 
struct add_lvalue_reference : detail::add_lvalue_reference_impl<T> {}; 

// add_rvalue_reference
namespace detail {
    template<typename T, typename = void> 
    struct add_rvalue_reference_impl {
        using type = T;
    }; 
    template<typename T>
    struct add_rvalue_reference_impl<T, decltype(void(std::declval<T&&>()))> {
        using type = T&&;
    };
}

template<typename T> 
struct add_rvalue_reference : detail::add_rvalue_reference_impl<T> {}; 

// 
//  Pointers
// 

// remove_pointer 
template<typename T> 
struct remove_pointer {
    using type = T; 
}; 

template<typename T> 
struct remove_pointer<T*> {
    using type = T;
};

template<typename T> 
struct remove_pointer<T* const> {
    using type = T;
};

template<typename T> 
struct remove_pointer<T* volatile> {
    using type = T;
};

template<typename T> 
struct remove_pointer<T* const volatile> {
    using type = T;
};

template<typename T> 
struct add_pointer: detail::add_pointer_impl<T> {}; 

// 
// OTHER TRANSFORMATIONS 
// 

// decay 
namespace detail {
    template<typename T> 
    struct decay_impl {
        using U = typename remove_reference<T>::type; 
        using type = typename std::conditional<
            is_array<U>::value, 
            typename remove_extent<U>::type*, 
            typename std::conditional<
                std::is_function<U>::value, 
                typename add_pointer<U>::type, 
                typename remove_cv<U>::type
            >::type
        >::type
    };
}
template<typename T> 
struct decay : detail::decay_immpl<T> {}; 

// enable_if 
template<bool B, typename T = void> 
struct enable_if {}; 

template<typename T>
struct enable_if<true, T> {
    using type = T;
};

// conditional 
template<bool B, typename T, typename F> 
struct conditional {
    using type = T;
}; 

template<typename T, typename F>
struct conditional<false, T, F> {
    using type = F;
};

// ===========
// C++14 Alias Template 
// ===========

template<typename T> 
using remove_const_t = typename remove_const<T>::type; 

template<typename T> 
using remove_volatile_t = typename remove_volatile_t<T>::type; 

template<typename T>
using remove_cv_t = typename remove_cv<T>::type;

template<typename T>
using add_const_t = typename add_const<T>::type;

template<typename T>
using add_volatile_t = typename add_volatile<T>::type;

template<typename T>
using add_cv_t = typename add_cv<T>::type;

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template<typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

template<typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

template<typename T>
using add_pointer_t = typename add_pointer<T>::type;

template<typename T>
using decay_t = typename decay<T>::type;

template<bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template<bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

// ==================
// C++17 Variable Templates (_v) 
// ==================

template<typename T> 
inline constexpr bool is_void_v = is_void<T>::value; 

template<typename T> 
inline constexpr bool is_integral_v = is_integral<T>::value; 

template<typename T> 
inline constexpr bool is_floating_point_v = is_floating_point<T>::value; 

template<typename T>
inline constexpr bool is_array_v = is_array<T>::value;

template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

template<typename T>
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template<typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template<typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

template<typename T>
inline constexpr bool is_arithmetic_v = is_arithmetic<T>::value;

template<typename T>
inline constexpr bool is_const_v = is_const<T>::value;

template<typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;

template<typename T, typename U>
inline constexpr bool is_same_v = is_same<T, U>::value;


}