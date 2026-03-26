#include <type_traits> 
#include <utility> 
#include <cstddef> 

namespace career {

    template<typename T> 
    class function; 

    template<typename R, typename... Args> 
    class function<R(Args...)> {
    private: 
        static constexpr size_t SBO_SIZE = 2 * sizeof(void*); 
        using byte = unsigned char; 

        union Storage {
            void* _ptr;
            alignas(std::max_align_t) byte _buffer[SBO_SIZE];
        }

        // ========================================================================
        // OPERATIONS - What the Manager Can Do
        // ========================================================================
        //
        // The Manager is a function pointer that "remembers" the type F.
        // These operations tell the manager what to do with the stored callable.
    
        enum class Operation {
            GET_TYPE_INFO, // return std::type_info of the stored type
            GET_POINTER,   // Return a pointer to the stored callable 
            CLONE,         // Copy construct the callable 
            MOVE,          // Move construct the callable 
            DESTROY        // Destruct the callable
        }; 

        using InvokerFunc = R(*)(const Storage&, Args&&...); 

        using ManagerFunc = void(*)(Operation op,
                                    Storage& dest,
                                    Storage& src, 
                                    void** ret_ptr, 
                                    const std::type_info** ret_type);
        
        Storage _storage;                  // Where the callable is stored 
        InvokerFunc _invoker = nullptr;    // How to call it 
        ManagerFunc _manager = nullptrl    // How to manage its life time 

        // ========================================================================
        // HELPER: Check if Type Fits in Small Buffer
        // ========================================================================
        //
        // For SBO to be safe, the type must:
        //   1. Fit in the buffer (size check)
        //   2. Be properly aligned (alignment check)
        //   3. Be trivially copyable (can use memcpy for simple moves)
        //
        // Why trivially copyable?
        //   - We need to swap function objects with std::swap
        //   - std::swap does a bitwise copy of the union
        //   - If the type isn't trivially copyable, this could corrupt it!
    
        template<typename F> 
        static constexpr bool _is_small() noexcept {
            using Decayed = std::decay_t<F>; 
            return sizeof(Decayed) <= SBO_SIZE &&
                   alignof(Decayed) <= alignof(std::max_allign_t) && 
                   std::is_trivially_copyable_v<Decayed>; 
        }

        // ========================================================================
        // INVOKER IMPLEMENTATIONS
        // ========================================================================
        //
        // These are TEMPLATE FUNCTIONS that get instantiated for each type F.
        // When we store a lambda, we create an invoker specifically for that lambda.
        // The invoker function pointer "bakes in" the type at compile time!
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ INVOKER FOR SMALL STORAGE (callable is in _buffer)                  │
        // └─────────────────────────────────────────────────────────────────────┘

        template<typename F> 
        static R _invoke_small(const Storage& storage, Args&&... args) {
            using Decayed = std::decay_t<F>; 
            const Decayed* func = reinterpret_cast<const Decayed*> (storage._buffer); 
            if constexpr (std::is_void_v<R>) {
                // void function cannot write return func(...)
                (*func)(std::forward<Args>(args)...);
            } else {
                // non-void return: forward the result 
                return (*func)(std::forward<Args>(args)...);
            }
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ INVOKER FOR LARGE STORAGE (callable is on heap, _ptr points to it)  │
        // └─────────────────────────────────────────────────────────────────────┘
        
        template<typename F> 
        static R _invoke_large(const Storage& storage, Args&&... args) {
            using Decayed = std::decay_t<F>; 
            const Decayed* func = reinterpret_cast<const Decayed*>(storage._ptr);
            if constexpr (std::is_void_v<R>) {
                (*func)(std::forward<Args>(args)...); 
            } else {
                return (*func)(std::forward<Args>(args)...);
            }
        }

        // ========================================================================
        // MANAGER IMPLEMENTATIONS
        // ========================================================================
        //
        // The Manager is the HEART of type erasure!
        //
        // Problem: After storing a callable, we've "forgotten" its type.
        //          How do we copy/move/destroy it?
        //
        // Solution: The manager function pointer "remembers" the type F.
        //           When instantiated for a specific F, it can cast back and
        //           perform type-specific operations.
        //
        // Example:
        //   auto lambda = [x=42](int y) { return x + y; };
        //   _manager = &_manage_small<decltype(lambda)>;  // Remembers the type!
        //   
        //   Later (type is "forgotten"):
        //   _manager(Operation::DESTROY, ...)  // Manager knows it's that lambda!
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ MANAGER FOR SMALL STORAGE                                            │
        // └─────────────────────────────────────────────────────────────────────┘
        template<typename F> 
        static void _manage_small(Operation op, Storage& dest, Storage& src, 
                                    void** ret_ptr, const std::type_info** ret_type) {
            using Decayed = std::decay_t<F>; 

            switch(op) {
            // ────────────────────────────────────────────────────────────────
            // GET_TYPE_INFO: Return the type_info of the stored type
            // ────────────────────────────────────────────────────────────────
            case Operation::GET_TYPE_INFO: {
                if (ret_type) {
                    *ret_type = &typeid(Decayed);
                }
                break; 
            }
            // ────────────────────────────────────────────────────────────────
            // GET_POINTER: Return pointer to the stored callable
            // Used by target() to access the underlying callable
            // ────────────────────────────────────────────────────────────────
            case Operation::GET_POINTER: {
                if (ret_ptr) {
                    // Need to cast away const because caller might want non-const pointer
                    *ret_ptr = const_cast<Decayed*>(reinterpret_cast<const Decayed*>(src._buffer));
                }
                break;
            }
            // ────────────────────────────────────────────────────────────────
            // CLONE: Copy construct from src to dest
            // Used by copy constructor and copy assignment
            // ────────────────────────────────────────────────────────────────
            case Operation::CLONE: {
                const Decayed* src_ptr = reinterpret_cast<const Decayed*>(src._buffer); 
                ::new (static_cast<void*>(dest._buffer)) Decayed(*src_ptr);
                break;
            }
            // ────────────────────────────────────────────────────────────────
            // MOVE: Move construct from src to dest, then destroy src
            // Used by move constructor and move assignment
            // ────────────────────────────────────────────────────────────────
            case Opeartion::MOVE: {
                Decayed* src_ptr = reinterpret_cast<Decayed*>(src._buffer); 
                ::new (static_case<void*>(dest._buffer)) Decayed(std::move(*src_ptr));
                src_ptr->~Decayed();
                break; 
            }
            // ────────────────────────────────────────────────────────────────
            // DESTROY: Call destructor on the stored object
            // Used by destructor and reset()
            // ────────────────────────────────────────────────────────────────
            case Operation::DESTROY: {
                Decayed* obj = reinterpret_cast<Decayed*>(src._buffer); 
                obj->~Decayed(); 
                break;
            }
            }
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ MANAGER FOR LARGE STORAGE (heap allocation)                          │
        // └─────────────────────────────────────────────────────────────────────┘
        
        template<typename F>
        static void _manage_large(Operation op, Storage& dest, Storage& src,
                                void** ret_ptr, const std::type_info** ret_type) {
            using Decayed = std::decay_t<F>;
            
            switch (op) {
            
            case Operation::GET_TYPE_INFO:
                if (ret_type) {
                    *ret_type = &typeid(Decayed);
                }
                break;
            
            case Operation::GET_POINTER:
                if (ret_ptr) {
                    // For heap storage, just return the pointer directly
                    *ret_ptr = src._ptr;
                }
                break;
            
            // ────────────────────────────────────────────────────────────────
            // CLONE: Allocate new object on heap and copy
            // ────────────────────────────────────────────────────────────────
            case Operation::CLONE: {
                const Decayed* src_ptr = static_cast<const Decayed*>(src._ptr);
                
                // Allocate new object on heap with copy constructor
                dest._ptr = new Decayed(*src_ptr);
                break;
            }
            
            // ────────────────────────────────────────────────────────────────
            // MOVE: Just transfer the pointer (cheap!)
            // ────────────────────────────────────────────────────────────────
            case Operation::MOVE:
                // Transfer ownership of heap allocation
                dest._ptr = src._ptr;
                src._ptr = nullptr;  // Null out source (moved-from)
                break;
            
            // ────────────────────────────────────────────────────────────────
            // DESTROY: Delete heap allocation
            // ────────────────────────────────────────────────────────────────
            case Operation::DESTROY:
                delete static_cast<Decayed*>(src._ptr);
                src._ptr = nullptr;
                break;
            }
        }

        // ========================================================================
        // CONSTRUCTION HELPER
        // ========================================================================
        //
        // This is where the MAGIC happens!
        // Based on the size of F, we decide:
        //   - Where to store it (buffer vs heap)
        //   - Which invoker to use (_invoke_small vs _invoke_large)
        //   - Which manager to use (_manage_small vs _manage_large)
        
        template<typename F>
        void _construct_impl(F&& f) {
            using Decayed = std::decay_t<F>;
            
            // ────────────────────────────────────────────────────────────────
            // COMPILE-TIME CHECKS
            // ────────────────────────────────────────────────────────────────
            
            // Verify F is callable with our signature
            // is_invocable_r checks: Can we call Decayed with Args... and get R?
            static_assert(std::is_invocable_r_v<R, Decayed&, Args...>,
                        "Callable must be invocable with function<R(Args...)> signature");
            
            // ────────────────────────────────────────────────────────────────
            // DECIDE: Small buffer or heap?
            // ────────────────────────────────────────────────────────────────
            
            if constexpr (_is_small<Decayed>()) {
                // ═══════════════════════════════════════════════════════════
                // SMALL PATH: Store inline in buffer (zero allocations!)
                // ═══════════════════════════════════════════════════════════
                
                // Placement new: construct F directly in buffer
                ::new (static_cast<void*>(&_storage._buffer)) Decayed(std::forward<F>(f));
                
                // Set up function pointers for this specific type
                _invoker = &_invoke_small<Decayed>;
                _manager = &_manage_small<Decayed>;
                
            } else {
                // ═══════════════════════════════════════════════════════════
                // LARGE PATH: Allocate on heap
                // ═══════════════════════════════════════════════════════════
                
                // Allocate and construct on heap
                _storage._ptr = new Decayed(std::forward<F>(f));
                
                // Set up function pointers for heap storage
                _invoker = &_invoke_large<Decayed>;
                _manager = &_manage_large<Decayed>;
            }
        }

        // ========================================================================
        // INTERNAL HELPERS
        // ========================================================================
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ _reset: Destroy stored callable and clear state                      │
        // └─────────────────────────────────────────────────────────────────────┘
        void _reset() noexcept {
            if (_manager) {
                // Ask manager to destroy the stored callable
                _manager(Operation::DESTROY, _storage, _storage, nullptr, nullptr);
                
                // Clear our state
                _invoker = nullptr;
                _manager = nullptr;
            }
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ _clear: Just clear state without destroying (for moved-from state)   │
        // └─────────────────────────────────────────────────────────────────────┘
        void _clear() noexcept {
            _invoker = nullptr;
            _manager = nullptr;
            _storage._ptr = nullptr;
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ _is_empty_function_pointer: Check if function pointer is null        │
        // └─────────────────────────────────────────────────────────────────────┘
        static bool _is_empty_function_pointer(R(*fp)(Args...)) {
            return fp == nullptr;
        }
    public: 
        // ========================================================================
        // CONSTRUCTORS
        // ========================================================================
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Default constructor: Creates empty function                          │
        // └─────────────────────────────────────────────────────────────────────┘
        function() noexcept = default;
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Null pointer constructor: Creates empty function                     │
        // └─────────────────────────────────────────────────────────────────────┘
        function(std::nullptr_t) noexcept : function() {}
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Function pointer constructor                                          │
        // └─────────────────────────────────────────────────────────────────────┘
        function(R(*fp)(Args...)) {
            // Don't store null function pointers
            if (!_is_empty_function_pointer(fp)) {
                _construct_impl(fp);
            }
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Universal constructor: Accepts ANY callable!                          │
        // │                                                                        │
        // │ This is the CORE feature - it accepts:                                │
        // │   - Lambdas (with or without captures)                                │
        // │   - Function objects (classes with operator())                        │
        // │   - std::bind expressions                                             │
        // │   - Function pointers                                                 │
        // │   - Member function pointers (wrapped)                                │
        // │                                                                        │
        // │ CRITICAL: The enable_if prevents this from being selected for         │
        // │           copy/move construction (would cause infinite recursion!)    │
        // └─────────────────────────────────────────────────────────────────────┘
        template<typename F,
                typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, function>>>
        function(F&& f) {
            _construct_impl(std::forward<F>(f));
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Copy constructor: Clone the stored callable                          │
        // └─────────────────────────────────────────────────────────────────────┘
        function(const function& other) {
            if (!other) {
                // Other is empty, make this empty too
                return;
            }
            
            // Copy the function pointers
            _invoker = other._invoker;
            _manager = other._manager;
            
            // Use manager to clone the stored callable
            // Manager knows the type and how to copy it!
            _manager(Operation::CLONE, _storage, 
                    const_cast<Storage&>(other._storage), nullptr, nullptr);
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Move constructor: Transfer ownership of the stored callable          │
        // └─────────────────────────────────────────────────────────────────────┘
        function(function&& other) noexcept {
            if (!other) {
                // Other is empty, nothing to move
                return;
            }
            
            // Transfer the function pointers
            _invoker = other._invoker;
            _manager = other._manager;
            
            // Use manager to move the stored callable
            // For small: moves bytes and destructs source
            // For large: transfers pointer
            _manager(Operation::MOVE, _storage, other._storage, nullptr, nullptr);
            
            // Clear the moved-from function
            other._clear();
        }

        // ========================================================================
        // DESTRUCTOR
        // ========================================================================
        
        ~function() {
            _reset();  // Destroy stored callable if any
        }
        
        // ========================================================================
        // ASSIGNMENT OPERATORS
        // ========================================================================
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Copy assignment: Uses copy-and-swap idiom for exception safety       │
        // └─────────────────────────────────────────────────────────────────────┘
        function& operator=(const function& other) {
            if (this == &other) {
                return *this;  // Self-assignment check
            }
            
            // Copy-and-swap idiom:
            // 1. Create temporary copy (might throw)
            // 2. Swap with this (noexcept)
            // 3. temp's destructor cleans up old value
            function temp(other);
            swap(temp);
            return *this;
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Move assignment: Transfer ownership                                   │
        // └─────────────────────────────────────────────────────────────────────┘
        function& operator=(function&& other) noexcept {
            if (this == &other) {
                return *this;  // Self-assignment check
            }
            
            // Destroy current contents
            _reset();
            
            if (!other) {
                return *this;  // Other is empty, we're done
            }
            
            // Transfer from other
            _invoker = other._invoker;
            _manager = other._manager;
            _manager(Operation::MOVE, _storage, other._storage, nullptr, nullptr);
            other._clear();
            
            return *this;
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Assignment from callable: Replace stored callable                     │
        // └─────────────────────────────────────────────────────────────────────┘
        template<typename F,
                typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, function>>>
        function& operator=(F&& f) {
            // Create new function and swap
            function temp(std::forward<F>(f));
            swap(temp);
            return *this;
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Assignment from nullptr: Clear the function                          │
        // └─────────────────────────────────────────────────────────────────────┘
        function& operator=(std::nullptr_t) noexcept {
            _reset();
            return *this;
        }
        
        // ========================================================================
        // SWAP
        // ========================================================================
        //
        // CRITICAL: This is why we need std::is_trivially_copyable!
        //
        // std::swap does a bitwise copy of the union. If the stored callable
        // has complex copy semantics (e.g., manages resources), this could
        // corrupt it. That's why we require trivially_copyable for SBO.
        
        void swap(function& other) noexcept {
            using std::swap;
            swap(_storage, other._storage);
            swap(_invoker, other._invoker);
            swap(_manager, other._manager);
        }

        // ========================================================================
        // RESET - Clear the function
        // ========================================================================
        
        void reset() noexcept {
            _reset();
        }
        
        // ========================================================================
        // CALL OPERATOR - The Whole Point!
        // ========================================================================
        //
        // This is what makes function "callable" with function(args...)
        //
        // Flow:
        //   1. Check if function is empty
        //   2. Call the invoker with our storage and arguments
        //   3. Invoker extracts the callable and calls it
        //   4. Return the result
        
        R operator()(Args... args) const {
            if (!_invoker) {
                throw bad_function_call();
            }
            
            // Delegate to the invoker
            // Invoker knows the real type and how to call it!
            return _invoker(_storage, std::forward<Args>(args)...);
        }

        // ========================================================================
        // OBSERVERS
        // ========================================================================
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Check if function is empty (bool conversion)                         │
        // └─────────────────────────────────────────────────────────────────────┘
        explicit operator bool() const noexcept {
            return _invoker != nullptr;
        }
        
        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Get type_info of stored callable                                     │
        // └─────────────────────────────────────────────────────────────────────┘
        const std::type_info& target_type() const noexcept {
            if (!_manager) {
                return typeid(void);  // Empty function
            }
            
            // Ask manager for type info
            const std::type_info* ti = nullptr;
            _manager(Operation::GET_TYPE_INFO, 
                    const_cast<Storage&>(_storage),
                    const_cast<Storage&>(_storage), 
                    nullptr, &ti);
            return *ti;
        }

        // ┌─────────────────────────────────────────────────────────────────────┐
        // │ Access the underlying callable (if type matches)                     │
        // │                                                                      │
        // │ Usage:                                                               │
        // │   auto lambda = [](int x) { return x * 2; };                         │
        // │   function<int(int)> f = lambda;                                     │
        // │   auto* ptr = f.target<decltype(lambda)>();  // Get pointer to lambda│
        // └─────────────────────────────────────────────────────────────────────┘
        template<typename T>
        T* target() noexcept {
            if (!_manager || target_type() != typeid(T)) {
                return nullptr;  // Type mismatch or empty
            }
            
            // Ask manager for pointer to callable
            void* p = nullptr;
            _manager(Operation::GET_POINTER, _storage, _storage, &p, nullptr);
            return static_cast<T*>(p);
        }
        
        template<typename T>
        const T* target() const noexcept {
            if (!_manager || target_type() != typeid(T)) {
                return nullptr;
            }
            
            void* p = nullptr;
            _manager(Operation::GET_POINTER, 
                    const_cast<Storage&>(_storage),
                    const_cast<Storage&>(_storage), 
                    &p, nullptr);
            return static_cast<const T*>(p);
        }

    }; 

    template<typename R, typename... Args>
    void swap(function<R(Args...)>& lhs, function<R(Args...)>& rhs) noexcept {
        lhs.swap(rhs);
    }

}; // namespace career 

// ============================================================================
// KEY TAKEAWAYS AND TRICKY POINTS
// ============================================================================
//
// 1. TYPE ERASURE PATTERN
//    ════════════════════
//    Problem: Need to store different types in the same container
//    Solution: Store void* + function pointers that "remember" the type
//    
//    The function pointers are instantiated from templates, so they "bake in"
//    the type at compile time, then can work with it at runtime.
//
// 2. SMALL BUFFER OPTIMIZATION (SBO)
//    ════════════════════════════════
//    Most lambdas are small (8-16 bytes). Allocating on heap for each one
//    would be slow. SBO stores small callables inline (zero allocations!).
//    
//    CRITICAL: Requires is_trivially_copyable because we use std::swap
//    which does bitwise copy of the union.
//
// 3. THE MANAGER PATTERN
//    ═══════════════════
//    The manager is a function pointer that performs type-specific operations
//    (copy, move, destroy) even after the type has been "erased".
//    
//    It works because template instantiation creates a unique manager function
//    for each type, and the function pointer "remembers" which one to call.
//
// 4. INVOKER PATTERN
//    ═══════════════
//    Similar to manager, but for calling the callable.
//    Extracts the callable from storage and calls it with given arguments.
//
// 5. PERFECT FORWARDING
//    ══════════════════
//    std::forward preserves value categories (lvalue/rvalue).
//    Critical for efficiency - allows move semantics when possible.
//
// 6. CONST-CORRECTNESS
//    ════════════════
//    operator() is const, but the callable might be mutable internally.
//    That's OK - we're not modifying the function object, just calling it.
//
// 7. EXCEPTION SAFETY
//    ════════════════
//    Copy assignment uses copy-and-swap for strong guarantee.
//    If copy throws, original is unchanged.
//
// 8. ENABLE_IF TRICK
//    ═══════════════
//    Prevents template constructor from hijacking copy/move constructors.
//    Without it, template would match better and cause infinite recursion!
//
// ============================================================================