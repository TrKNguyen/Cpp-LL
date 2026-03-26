#include <cstddef> 
#include <memory> 

namespace career {

// =================================
// POINTER REBIND 
// =================================
template<typename Ptr, typename U> 
using ptr_rebind = typename std::pointer_traits<Ptr>::template rebind<U>; 

// =================================
// CONFIGURATION CONSTANTS
// =================================

constexpr int DEQUE_BUFFER_SIZE = 512;

inline constexpr size_t calculate_buffer_size(size_t element_size) {
    return size_t(DEQUE_BUFFER_SIZE) < element_size ? size_t(1) : size_t(DEQUE_BUFFER_SIZE / element_size); 
}

// =================================
// DEQUE ITERATOR 
// =================================

template<typename T, typename Reference, typename Pointer> 
struct DequeIterator {
    using iterator_category = std::random_access_iterator_tag; 
    using value_type = T; 
    using pointer = Pointer; 
    using reference = Reference; 
    using size_type - size_t; 
    using difference_type = ptrdiff_t; 

    using ElementPointer = ptr_rebind<Pointer, T>; 
    using MapPointer = ptr_rebind<Pointer, ElementPointer>; 
    using Self = DequeIterator;

    ElementPointer current; // current element 
    ElementPointer first; // first element within current buffer
    ElementPointer last; // last element within current buffer 
    MapPointer node; // pointer to the node in the map(this node points to buffer)

    static size_t buffer_size() noexcept {
        return calculate_buffer_size(sizeof(value_type));
    }

    // Constructors
    DequeIterator(ElementPointer x, MapPointer y) noexcept
        : current(x), first(*y), last(*y + buffer_size()), node(y){}

    DequeIterator() noexcept
        : current(), first(), last(), node(){}
    
    // Deference Operators
    reference operator*() const noexcept {
        return *current;
    }

    pointer operator->() const noexcept {
        return current;
    }
    // pre-increment operator: ++it
    Self& operator++() noexcept {
        current++; 
        if (current == last) {
            set_node(node + 1);
            current = *node;
        }
        return *this;
    }

    // post-increment operator: it++
    Self operator++(int) noexcept {
        Self tmp = *this;
        ++*this;
        return tmp;
    }

    Self& operator--() noexcept{
        if (current == first) {
            set_node(node - 1); 
            current = last; 
        }
        current--;
        return *this;
    }

    Self operator--(int) noexcept {
        Self tmp = *this; 
        --*this; 
        return tmp;
    }

    // Random access: jump multiple elements at once 
    Self& operator+=(difference_type n) noexcept {
        const difference_type offset = n + (current - first); 
        if (offset >= 0 && offset < difference_type(buffer_size())) {
            current += n;
        } else {
            const difference_type node_offset = 
                offset > 0 
                ?   offset / difference_type(buffer_size())
                :   -difference_type((-offset - 1) / difference_type(buffer_size()) + 1);
            set_node(node + node_offset); 
            current = first + (offset - node_offset * difference_type(buffer_size()));
        }
        return *this;
    }
    Self& operator-=(difference_type n) noexcept {
        return *this += -n;
    }
    Self operator+(difference_type n) noexcept {
        Self tmp = *this; 
        return tmp += n;
    }
    Self operator-(difference_type n) noexcept {
        Self tmp = *this; 
        return tmp -= n;
    }
    reference operator[](difference_type n) noexcept {
        return (*(*this + n));
    }
    friend Self operator+(difference_type n, const Self& it) {
        return it + n;
    }
    // Set to a new node(point to a new buffer)
    void set_node(MapPointer new_node) noexcept {
        node = new_node; 
        first = *node; 
        last = *node + difference_type(buffer_size());
    }

    // Comparison operators 
    bool operator==(const Self& other) const noexcept {
        return current == other.current;
    }
    bool operator!=(const Self& other) const noexcept {
        return !(*this == other);
    }
    bool operator>(const Self& other) const noexcept {
        return (node > other.node) || (node == other.node && current > other.current);
    }
    bool operator<(const Self& other) const noexcept {
        return other > *this;
    }
    bool operator>=(const Self& other) const noexcept {
        return !(*this < other);
    }
    bool operator<=(const Self& other) const noexcept {
        return !(*this > other);
    }
    difference_type operator-(const Self& other) const noexcept {
        return (node - other.node - 1) * difference_type(buffer_size())  
                + (current - first)
                + (other.last - other.current);
    }

}; 
// template<typename T, typename Reference, typename Pointer> 
// inline DequeIterator<T, Reference, Pointer>
// operator+(ptrdiff_t n, const DequeIterator<T, Reference, Pointer>& it) {
//     return it + n;
// }

template<typename T, typename Allocator = std::allocator<T>> 
class DequeBase {
protected: 
    using allocator_type = Allocator; 
    using allocator_traits = std::allocator_traits<Allocator>; 
    using pointer = typename allocator_traits::pointer; 

    using MapPointer = ptr_rebind<pointer, pointer> 

    static constexpr size_t INITIAL_MAP_SIZE = 8; 

    struct DequeData {
        MapPointer map; 
        size_t map_size; 
        DequeIterator<T, T&, pointer> start; 
        DequeIterator<T, T&, pointer> finish;

        DequeData() noexcept
            : map(nullptr), map_size(0), start(), finish() {}
    }

    Allocator allocator; 
    DequeData data; 

    DequeBase() = default;

    explicit DequeBase(const Allocator& alloc): allocator(alloc) {}

    DequeBase(const Allocator& alloc, size_t num_elements)
        : allocator(alloc) {
        initialize_map(num_elements); 
    }
    ~DequeBase() noexcept {
        if (data.map) {
            destroy_nodes(data.start, data.finish + 1); 
            deallocate_map(data.map, data.map_size);
        }
    }

    pointer allocate_node() {
        return std::allocator_traits<Allocator>::allocate(allocator, calculate_buffer_size(sizeof(T))); 
    }

    void deallocate_node(pointer p) noexcept {
        std::allocator_traits<Allocator>::deallocate(allocator, p, calculate_buffer_size(sizeof(T)));
    }

    MapPointer allocate_map(size_t n) {
        std::allocator_traits<Allocator>::template rebind_alloc<pointer> map_alloc(allocator); 
        return std::allocator_traits<std::allocator_traits<Allocator>::template rebind_alloc<pointer>>::allocate(map_alloc, n);
    }
    
    void deallocate_map(MapPointer p, size_t n) noexcept {
        std::allocator_traits<Allocator>::template rebind_alloc<pointer> map_alloc(allocator); 
        std::allocator_traits<std::allocator_traits<Allocator>::template rebind_alloc<pointer>>::deallocate(map_alloc, p, n);
    }

    void initialize_map(size_t num_elements) {
        const size_t num_nodes = num_elements / calculate_buffer_size(sizeof(T)) + 1;
        data.map_size = std::max(INITIAL_MAP_SIZE, num_nodes + 2); 
        data.map = allocate_map(data.map_size);

        MapPointer nstart = data.map + (data.map_size - num_nodes) / 2; 
        MapPointer nfinish = nstart + num_nodes;

        try {
            create_nodes(nstart, nfinish); 
        } catch(...) {
            deallocate_map(data.map, data.map_size); 
            data.map = nullptr;
            data.map_size = 0; 
            throw;
        }

        data.start.set_node(nstart);
        data.finish.set_node(nfinish - 1);
        data.start.current = data.start.first; 
        data.finish.current = data.finish.first + num_elements % calculate_buffer_size(sizeof(T));
    }   

    void create_nodes(MapPointer nstart, MapPointer nfinish) {
        MapPointer cur; 
        try {
            for (cur = nstart; cur < nfinish; cur++) {
                *cur = allocate_node();
            }
        } catch(...) {
            destroy_nodes(nstart, cur);
            throw;
        }
    }

    void destroy_nodes(MapPointer start, MapPointer finish) {
        for (auto it = start; it < finish; it++) {
            deallocate_node(*it);
        }
    }
};

template<typename T, typename Allocator = std::allocator<T>>
class Deque: protected DequeBase<T, Allocator> {
private: 
    using Base = DequeBase<T, Allocator>; 
public: 
    // =================
    // Type Definitions 
    // =================
    using value_type = T; 
    using allocator_type = Allocator; 
    using size_type = size_t; 
    using difference_type = ptrdiff_t; 
    using referene = T&; 
    using const_reference = const T&; 
    using pointer = typename std::allocator_traits<Allocator>::pointer; 
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = std::reverse_iterator<iterator>; 
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Default constructor 
    Deque() = default; 

    // Allocator constructor 
    explicit Deque(const Allocator& alloc)
        : Base(alloc, 0) {}

    explicit Deque(size_type count, const Allocator& alloc = Allocator()) 
        : Base(alloc, count) {
        default_initialize();
    }

    Deque(size_type count, const T& value, const Allocator& alloc = Allocator()) 
        : Base(alloc, check_size(count)) {
        fill_initialize(value);
    }

    // Range constructor 
    template<typename InputIterator, 
             typename = std::enable_if_t<
                std::is_base_of_v<std::input_iterator_tag,
                    typename std::iterator_traits<InputIterator>::iterator_category>>>
    Deque(InputIterator first, InputIterator last, const Allocator& alloc = Allocator()): Base(alloc) {
        range_initialize(first, last, typename std::iterator_traits<InputIterator>::iterator_category{}); 
    }

    // Copy constructor 
    Deque(const Deque& other)
        : Base(std::allocator_traits<Allocator>::select_on_container_copy_construction(
            other.get_allocator_ref(), other.size())) {
        std::uninitialized_copy(other.begin(), other.end(), this->data.start);
    }

    // Copy constructor with allocator 
    Deque(const Deque& other, const Allocator& alloc)
        : Base(alloc, other.size()) {
        std::uninitialized_copy(other.begin(), other.end(), this->data.start);
    }

    // Move constructor
    Deque(Dequeu&& other)
        : Base(std::move(other.get_allocator_ref())) noexcept {
        this->initialize_map(0); 
        if (other.data.map) {
            this->data.swap_data(other.data);
        }
    }

    // Move constructor with allocator 
    Deque(Deque&&other, const Allocator& alloc): Base(alloc) {
        if (other.get_allocator_ref() == alloc) {
            // Allocators match can steal resources 
            this->initialize_map(0); 
            if (other.data.map) {
                this->data.swap_data(other.data);
            }
        } else {
            // Allocators differ - must move elements 
            this->initialize_map(other.size()); 
            std::uninitialized_copy(
                std::make_move_iterator(other.begin()), 
                std::make_move_iterator(other.end()),
                this->data.start);
            other.clear();
        }
    }

    // Initializer list constructor 
    Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator()) 
        : Base(alloc) {
        range_initialize(init.begin(), init.end(), std::random_access_iterator_tag{});
    }

    // Destructor 
    ~Deque() {
        destroy_data(begin(), end()); 
    }

    // Assignment Operators

    // Copy assignment 
    Deque& operator=(const Deque& other) {
        if (this != &other) {
            const size_type len = size(); 
            if (len >= other.size()) {
                erase_at_end(std::copy(other.begin(), other.end(), begin()));
            } else {
                const_iterator mid = other.begin() + difference_type(len);
                std::copy(other.begin(), mid, begin());
                insert(end(), mid, other.end());
            }
        }
        return *this;
    }
    
    // Move assignment 
    Deque& operator=(Deque&& other) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value) {
        // Move with propagation handling 
        constexpr bool propagate = std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value; 
        move_assign(std::move(other, std::bool_constant<propagate>{}));
        return *this;
    }

    // Initializer list assignment 
    Deque& operator=(std::initailizer_list<T> init) {
        assign(init.begin(), init.end()); 
        return *this; 
    }

    // Assign functions 
    void assign(size_type count, const T& value) {
        fill_assign(coutn, value);
    }

    template<typename InputIterator, 
             typename - std::enable_if_t<
                std::is_base_of_v<std::input_iterator_tag, 
                    typename std::iterator_traits<InputIterator>::iterator_category>>> 
    void assign(InputIterator first, InputIterator last) {
        assign_aux(first, last, typename std::iterator_traits<InputIterator>::iterator_category{}); 
    }
    void assign(std::initializer_list<T> init) {
        assign(init.begin(), init.end());
    }

    // Allocator 
    allocator_type get_allocator() const noexcept {
        return allocator_type(this->allocator);
    }

    // Iterators 
    iterator begin() noexcept {
        return this->data.start;
    }

    const_iterator begin() const noexcept {
        return this->data.start;
    }

    iterator end() noexcept {
        return this->data.finish;
    }
    const_iterator end() const noexcept {
        return this->data.finish;
    }
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    
    const_iterator cbegin() const noexcept {
        return this->data.start;
    }
    
    const_iterator cend() const noexcept {
        return this->data.finish;
    }
    
    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }
   
    [[no discard]] bool empty() const noexcept {
        return this->data.finish == this->data.start;
    }

    [[nodiscard]] size_type size() const noexcept {
        return this->data.finish - this->data.start;
    }

    [[nodiscard]] size_type max_size() const noexcept {
        const size_type diff_max = std::numeric_limits<difference_type>::max(); 
        const size_type alloc_max = std::allocator_traits<Allocator>::max_size(this->allocator); 
        return std::min(diff_max, alloc_max);
    }

    void resize(size_type new_size) {
        const size_type len = size(); 
        if (new_size > len) {
            default_append(new_size - len); 
        } else if (new_size < len) {
            erase_at_end(this->data.start + difference_type(new_size));
        }
    }
    void resize(size_type new_Size, const value_type& value) {
        const size_type len = size(); 
        if (new_size > ken) {
            insert(end(), new_size - len, value);
        }
    }
    void shrink_to_fit() noexcept {
        // Try to reduce memory usage
        // (Implementation can be no-op or actually shrink)
        // For now, simple implementation
    }
    
    // ========================================================================
    // Element Access
    // ========================================================================
    
    reference operator[](size_type n) noexcept {
        return this->data.start[difference_type(n)];
    }
    
    const_reference operator[](size_type n) const noexcept {
        return this->data.start[difference_type(n)];
    }
    
    reference at(size_type n) {
        range_check(n);
        return (*this)[n];
    }
    
    const_reference at(size_type n) const {
        range_check(n);
        return (*this)[n];
    }
    
    reference front() noexcept {
        return *begin();
    }
    
    const_reference front() const noexcept {
        return *begin();
    }
    
    reference back() noexcept {
        iterator tmp = end();
        --tmp;
        return *tmp;
    }
    
    const_reference back() const noexcept {
        const_iterator tmp = end();
        --tmp;
        return *tmp;
    }
    
    // Modifiers - Push / Pop 

    void push_front(const T& value) {
        if (this->data.start.current != this->data.start.first) {
            std::allocator_traits<Allocator>::construct(this->allocator, this->data.start.current - 1, value);
        } else {
            push_front_aux(value);
        }
    }
    void push_front(T&& value) {
       emplace_front(std::move(value)); 
    }

    template<typename... Args> 
    reference emplace_front(Args&&... args) {
        if (this->data.start.current != this->data.start.first) {
            std::allocator_traits<Allocator>::construct(this->allocator, this->data.start.current - 1, std::forward<Args>(args)...);
            --this->data.start.current;
        } else {
            emplace_front_aux(std::forward<Args>(args)...); 
        }
        return front();
    }
      void push_back(const T& value) {
        if (this->data.finish.current != this->data.finish.last - 1) {
            // Space available in current buffer
            allocator_traits::construct(this->allocator,
                                       this->data.finish.current,
                                       value);
            ++this->data.finish.current;
        } else {
            // Need new buffer
            push_back_aux(value);
        }
    }
    
    void push_back(T&& value) {
        emplace_back(std::move(value));
    }
    
    template<typename... Args>
    reference emplace_back(Args&&... args) {
        if (this->data.finish.current != this->data.finish.last - 1) {
            allocator_traits::construct(this->allocator,
                                       this->data.finish.current,
                                       std::forward<Args>(args)...);
            ++this->data.finish.current;
        } else {
            emplace_back_aux(std::forward<Args>(args)...);
        }
        return back();
    }

    void pop_front() {
        if (this->data.start.current != this->data.start.last - 1) {
            allocator_traits::destrou(this->allocator, this->data.start.current); 
            ++this->data.start.current;
        } else {
            pop_front_aux();
        }
    }

    void pop_back() noexcept {
        if (this->data.finish.current != this->data.finish.first) {
            --this->data.finish.current;
            allocator_traits::destroy(this->allocator, this->data.finish.current);
        } else {
            pop_back_aux();
        }
    }

    // ========================================================================
    // Modifiers - Insert/Emplace
    // ========================================================================
    
    template<typename... Args>
    iterator emplace(const_iterator position, Args&&... args) {
        if (position.current == this->data.start.current) {
            emplace_front(std::forward<Args>(args)...);
            return begin();
        } else if (position.current == this->data.finish.current) {
            emplace_back(std::forward<Args>(args)...);
            return end() - 1;
        } else {
            return insert_aux(position.const_cast_to_iterator(), 
                            std::forward<Args>(args)...);
        }
    }
    
    iterator insert(const_iterator position, const T& value) {
        return emplace(position, value);
    }
    
    iterator insert(const_iterator position, T&& value) {
        return emplace(position, std::move(value));
    }
    
    iterator insert(const_iterator position, size_type count, const T& value) {
        difference_type offset = position - cbegin();
        fill_insert(position.const_cast_to_iterator(), count, value);
        return begin() + offset;
    }
    
    template<typename InputIterator,
             typename = std::enable_if_t<
                 std::is_base_of_v<std::input_iterator_tag,
                     typename std::iterator_traits<InputIterator>::iterator_category>>>
    iterator insert(const_iterator position, InputIterator first, InputIterator last) {
        difference_type offset = position - cbegin();
        range_insert_aux(position.const_cast_to_iterator(), first, last,
                        typename std::iterator_traits<InputIterator>::iterator_category{});
        return begin() + offset;
    }
    
    iterator insert(const_iterator position, std::initializer_list<T> init) {
        return insert(position, init.begin(), init.end());
    }
    
    // ========================================================================
    // Modifiers - Erase
    // ========================================================================
    
    iterator erase(const_iterator position) {
        return erase_aux(position.const_cast_to_iterator());
    }
    
    iterator erase(const_iterator first, const_iterator last) {
        return erase_aux(first.const_cast_to_iterator(), 
                        last.const_cast_to_iterator());
    }
    
    // ========================================================================
    // Modifiers - Other
    // ========================================================================
    
    void swap(Deque& other) noexcept(
        allocator_traits::is_always_equal::value) {
        this->data.swap_data(other.data);
        std::swap(this->allocator, other.allocator);
    }
    
    void clear() noexcept {
        erase_at_end(begin());
    }
    
private:
    // ========================================================================
    // Helper Functions - Will be implemented in next steps
    // ========================================================================
    
    // Allocator access
    Allocator& get_allocator_ref() noexcept { return this->allocator; }
    const Allocator& get_allocator_ref() const noexcept { return this->allocator; }
    
    // Size checking
    static size_type check_size(size_type n) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T)) {
            throw std::length_error("Deque size exceeds maximum");
        }
        return n;
    }
    
    void range_check(size_type n) const {
        if (n >= size()) {
            throw std::out_of_range("Deque::at: index out of range");
        }
    }
    
    // Initialization helpers
    void default_initialize();
    void fill_initialize(const T& value);
    
    template<typename InputIterator>
    void range_initialize(InputIterator first, InputIterator last,
                         std::input_iterator_tag);
    
    template<typename ForwardIterator>
    void range_initialize(ForwardIterator first, ForwardIterator last,
                         std::forward_iterator_tag);
    
    // Assignment helpers
    void fill_assign(size_type count, const T& value);
    
    template<typename InputIterator>
    void assign_aux(InputIterator first, InputIterator last,
                   std::input_iterator_tag);
    
    template<typename ForwardIterator>
    void assign_aux(ForwardIterator first, ForwardIterator last,
                   std::forward_iterator_tag);
    
    // Move assignment helpers
    void move_assign(Deque&& other, std::true_type) noexcept;
    void move_assign(Deque&& other, std::false_type);
    
    // Push/pop aux functions
    void push_front_aux(const T& value);
    void push_back_aux(const T& value);
    
    template<typename... Args>
    void emplace_front_aux(Args&&... args);
    
    template<typename... Args>
    void emplace_back_aux(Args&&... args);
    
    void pop_front_aux();
    void pop_back_aux();
    
    // Insert/erase aux functions
    template<typename... Args>
    iterator insert_aux(iterator position, Args&&... args);
    
    void fill_insert(iterator position, size_type count, const T& value);
    
    template<typename InputIterator>
    void range_insert_aux(iterator position, InputIterator first, InputIterator last,
                         std::input_iterator_tag);
    
    template<typename ForwardIterator>
    void range_insert_aux(iterator position, ForwardIterator first, ForwardIterator last,
                         std::forward_iterator_tag);
    
    iterator erase_aux(iterator position);
    iterator erase_aux(iterator first, iterator last);
    
    // Resize helpers
    void default_append(size_type count);
    
    // Cleanup helpers
    void destroy_data(iterator first, iterator last);
    void erase_at_end(iterator position);
    
    // Memory management helpers  
    iterator reserve_elements_at_front(size_type n);
    iterator reserve_elements_at_back(size_type n);
    void new_elements_at_front(size_type n);
    void new_elements_at_back(size_type n);
    void reserve_map_at_back(size_type nodes_to_add = 1);
    void reserve_map_at_front(size_type nodes_to_add = 1);
    void reallocate_map(size_type nodes_to_add, bool add_at_front);
};

// ============================================================================
// Non-Member Functions
// ============================================================================

template<typename T, typename Alloc>
inline bool operator==(const Deque<T, Alloc>& lhs, const Deque<T, Alloc>& rhs) {
    return lhs.size() == rhs.size() && 
           std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Alloc>
inline bool operator!=(const Deque<T, Alloc>& lhs, const Deque<T, Alloc>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Alloc>
inline bool operator<(const Deque<T, Alloc>& lhs, const Deque<T, Alloc>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                       rhs.begin(), rhs.end());
}

template<typename T, typename Alloc>
inline bool operator<=(const Deque<T, Alloc>& lhs, const Deque<T, Alloc>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Alloc>
inline bool operator>(const Deque<T, Alloc>& lhs, const Deque<T, Alloc>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Alloc>
inline bool operator>=(const Deque<T, Alloc>& lhs, const Deque<T, Alloc>& rhs) {
    return !(lhs < rhs);
}

template<typename T, typename Alloc>
inline void swap(Deque<T, Alloc>& lhs, Deque<T, Alloc>& rhs) 
    noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace custom