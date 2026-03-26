#include <utility> 
#include <iterator>
#include <new> 

namespace career {

// ==================================================
// destroy - call desctructors on a range 
// ==================================================

template<typename ForwardIt> 
void destroy(ForwardIt first, ForwardIt last) noexcept {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    for(; first != last; ++first) {
        std::addressof(*first)->~value_type();
    }
}

template<typename ForwardIt, typename Size> 
ForwardIt destroy_n(ForwardIt first, Size n) noexcept {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    for(; n > 0; n--, first++) {
        std::addressof(*first)->~value_type();
    }
    return first;
}   

// ===================================================
// uninitialized_copy - Copy into uninitialized memory 
// ===================================================

template<typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = d_first; 
    try {
        for (; first != last; ++first, ++current) {
            ::new(static_cast<void*>(std::addressof(*current))) value_type(*first);
        }
        return current;
    } catch(...) {
        destroy(d_first, current);
        throw;
    }
}

template<typename InputIt, typename Size, typename ForwardIt> 
ForwardIt uninitialized_copy_n(InputIt first, Size n, ForwardIt d_first) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = d_first; 
    try {
        for (; n > 0; n--, current++, first++) {
            ::new(static_cast<void*>(std::addressof(*current))) value_type(*first);
        }
        return current;
    } catch(...) {
        destroy(d_first, current); 
        throw;
    }
}

// ============================================================================
// uninitialized_fill - Fill uninitialized memory
// ============================================================================

template<typename ForwardIt, typename T> 
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = first; 
    try {
        for (; current != last; current++) {
            ::new (static_cast<void*>(std::addressof(*current))) value_type(value);
        }
    } catch(...) {
        destroy(first, current); 
        throw;
    }
}


template<typename ForwardIt, typename Size, typename T> 
ForwardIt uninitialized_fill_n(ForwardIt first, Size n, const T& value) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = first;
    try {
        for (; n > 0; n--, current++) {
            ::new (static_cast<void*>(std::addressof(*current))) value_type(value);
        }
        return current;
    } catch(...) {
        destroy(first, current);
        throw;
    }
}

// ============================================================================
// uninitialized_move - Move into uninitialized memory (C++17)
// ============================================================================

template<typename InputIt, typename ForwardIt>
ForwardIt uninitialized_move(InputIt first, InputIt last, ForwardIt d_first) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = d_first; 
    try {
        for (; first != last; first++, current++) {
            ::new(static_cast<void*>(std::addressof(*current))) value_type(std::move(*first));
        }
        return current;
    } catch(...) {
        destroy(d_first, current); 
        throw; 
    }
}

template<typename InputIt, typename Size, typename ForwardIt>
std::pair<InputIt, ForwardIt> uninitialized_move_n(InputIt first, Size n, ForwardIt d_first) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = d_first;
    try {
        for (; n > 0; n--, current++, first++) {
            ::new(static_cast<void*>(std::addressof(*current)))value_type(std::move(*first));
        }
        return {first, current};
    } catch(...) {
        destroy(d_first, current);
        throw; 
    }
}

// ============================================================================
// uninitialized_default_construct - Default construct (C++17)
// ============================================================================

template<typename ForwardIt> 
void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = first; 
    try {
        for (; current != last ;current++) {
            ::new(static_cast<void*>(std::addressof(*current)))value_type;
        }
    } catch(...) {
        destroy(first, current);
        throw; 
    }
}

template<typename ForwardIt, typename Size> 
void uninitialized_default_construct_n(ForwardIt first, Size n) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = first; 
    try {
        for (; n > 0; n--, current++) {
            ::new(static_cast<void*>(std::addressof(*current)))value_type;
        }
    } catch(...) {
        destroy(first, current); 
        throw;
    }
}

// ============================================================================
// uninitialized_value_construct - Value construct (C++17)
// ============================================================================

template<typename ForwardIt> 
void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = first; 
    try {
        for (; current != last ;current++) {
            ::new(static_cast<void*>(std::addressof(*current)))value_type();
        }
    } catch(...) {
        destroy(first, current);
        throw; 
    }
}

template<typename ForwardIt, typename Size> 
void uninitialized_value_construct_n(ForwardIt first, Size n) {
    using value_type = typename std::iterator_traits<ForwardIt>::value_type; 
    ForwardIt current = first; 
    try {
        for (; n > 0; n--, current++) {
            ::new(static_cast<void*>(std::addressof(*current)))value_type();
        }
    } catch(...) {
        destroy(first, current); 
        throw;
    }
}

}