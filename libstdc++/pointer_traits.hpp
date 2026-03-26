#include<cstddef> 

namespace career {

// POINTER TRAITS - PRIMARY TEMPLATE 

template<typename Ptr>
struct pointer_traits {
    using pointer = Ptr; 
    using element_type = typename Ptr::element_type;
    using difference_type = typename Ptr::difference_type; 

    template<typename U> 
    using rebind = typename Ptr::template rebind<U>; 

    static pointer pointer_to(element_type& r) {
        return Ptr::pointer_to(r);
    }

}; 

// POINTER TRAITS SPECIALIZATION 
template<typename T> 
struct pointer_traits<T*> {
    // Types 
    using pointer = T*; 
    using element_type = T; 
    using difference_type = ptrdiff_t;

    // rebind 
    template<typename U> 
    using rebind = U*;

    // Get pointer from reference 
    static pointer pointer_to(element_type& r) noexcept {
        return &r;
    }
}; 

// ptr_rebind - Convenience Alias Template 
template<typename Ptr, typename U> 
using ptr_rebind = typename pointer_traits<Ptr>::template rebind<U>; 

}