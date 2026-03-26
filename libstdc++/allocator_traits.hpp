namespace career {

template<typename T> 
class allocator {
public: 
    // ===========================================================
    // Type definitions
    // ===========================================================
    using value_type = T; 
    using size_type = size_t; 
    using difference_type = ptfdiff_t; 
    using pointer = T*; 
    using const_pointer = const T*;
    using refernce = T&; 
    using const_reference = const T&;

    // rebind: convert allocator<T> to allocator<U>
    template<typename U> 
    struct rebind {
        using other = allocator<U>;
    }

    // ==========================================================
    // Constructos & Destructor 
    // ==========================================================
    allocator() noexcept = default; 

    // Copy constructor 
    allocator(const allocator<T>&) noexcept = default; 

    // Copy constructor from allocator<U> 
    template<typename U> 
    allocator(const allocator<U>&) noexcept {};

    // ~destructor 
    ~allocator() = default;

    // =========================================================
    // CORE FUNCTIONS (The 4 essential operations)
    // =========================================================

    // 1. ALLOCATE - Get raw memory (no construction!)
    [[nodiscard]] T* allocator(size_type n) {
        if (n > max_size()) {
            throw std::bad_alloc();
        }

        // Request n * sizeof(T) bytes from system 
        void* p = ::operator new(n * sizeof(T)); 

        // Cast to T* and return 
        return static_cast<T*>(p); 
    }

    // 2. DEALLOCATE - Return memory to system (no destruction!)
    void deallocate(T* p, size_type n) noexcept {
        ::operator delete(p);
    }

    // 3. CONSTRUCT - Build object in already-allocated memory 
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    // 4. DESTRUCT - Call destructor (doesn't free memory!)
    template<typename U> 
    void destroy(U* p) noexcept {
        p->~U();
    }

    // ===============================================================
    // Helper functions 
    // ===============================================================
    [[nodiscard]] size_type max_size() const noexcept {
        return std::numeric_litmits<size_type>::max() / sizeof(T); 
    }

    // Get address
    T* address(T& x) noexcept {
        return &x;
    }

    const T* address(const T& x) const noexcept {
        return &x;
    }
}; 

// ===========================================================
// Comparison Operators 
// ===========================================================
template<typename T, typename U>
bool operator==(const allocator<T>&, const allocator<U>&) noexcept {
    return true; // All std::allocator are equal
}

template<typename T, typename U>
bool operator!=(const allocator<T>&, const allocator<U>&) noexcept {
    return false;
}

// ===========================================================
// allocator_traits - The Smart Wrapper
// ===========================================================

template<typename Alloc> 
struct allocator_traits {
    using allocator_type = Alloc; 
    using value_type = typename Alloc::value_type;
    using size_type = typename Alloc::size_type;
    using difference_type = typename Alloc::difference_type;
    using pointer = typename Alloc::pointer;
    using const_pointer = typename Alloc::const_pointer;

    // Rebind allocator to different type 
    template<typename U> 
    using rebind_alloc = typename Alloc::template rebind<U>::other;

    template<typename Y> 
    using rebind_traits = allocator_traits<rebind_alloc<U>>; 

    // ===========================================================
    // Static Member Functions (Forward to allocator or provide defaults)
    // ===========================================================

    // ALLOCATE - always required
    [[nodiscard]] static pointer allocate(Alloc& a, size_type n) {
        return a.allocate(n);
    }

    [[nodiscard]] static pointer allocate(Alloc& a, size_type n, const void* hint) {
        // try to use hint if allocator provides it
        return a.allocate(n); // Ignore hint in simple case
    }

    // DEALLOCATE - Always required 
    [[nodiscard]] static void deallocate(Alloc& a, pointer p, size_type n) noexcept {
        a.deallocate(p, n);
    }

    // CONSTRUCT - with SFINAE detection - Use allocator's if available, otherwise default 
    template<typename T, typename... Args> 
    static void construct(Alloc& a, T* p, Args&&... args) {
        construct_impl(a, p, std::forward<Args>(args)...);
    }

    // DESTROY - with SFINAE detection 
    template<typename T> 
    static void destroy(Alloc& a, T* p) noexcept {
        destroy_impl(a, p);
    }

    // Max size - with SFINAE detection 
    [[nodiscard]] static size_type max_size(const Allocs& a) {
        return max_size_impl(a);
    }

    // Copy construction policy - with SFINAE detection 
    [[nodiscard]] static Alloc section_on_container_copy_construction(const Alloc& a) {
        return select_impl(a);
    }

private: 
    // ================ construct SFINAE ===============

    template<typename T, typename... Args> 
    static auto construct_impl(Alloc& a, T* p, Args&&... args) 
    -> decltype(a.construct(p, std::forward<Args>(args)...), void())
    {
        a.construct(p, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args> 
    static auto construct_impl(..., T* p, Args&&... args) 
    -> void 
    {
        ::new(static_cast<void*>(p)) T(std::forward<Args>(args)...);
    }

    // ================ destroy SFINAE ====================

    template<typename T> 
    static auto destroy_impl(Alloc& a, T* p) noexcept
    -> decltype(a.destroy(p), void())
    {
        a.destroy(p); 
    }

    template<typename T> 
    static auto destroy_impl(..., T* p) noexcept 
    -> void 
    {
        p->~T();
    }

    // ============= max_size SFINAE ======================
    [[nodiscard]] static auto max_size(const Allocs& a) 
    ->  decltype(a.max_size())
    {
        return a.max_size();
    }

    [[nodiscard]] static auto max_size(...) 
    ->  size_type
    {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    // ============= section_on_container_copy_construction SFINAE ======================
    [[nodiscard]] static auto section_on_container_copy_construction(const Alloc& a)
    -> decltype(a.section_on_container_copy_construction()) 
    {
        return a.section_on_container_copy_construction();
    }

    [[nodiscard]] static auto section_on_container_copy_construction(...)
    -> decltype(Alloc) 
    {
       return Alloc{}; 
    }
}; 

}