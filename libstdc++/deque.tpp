// deque.tpp - Implementation file for Deque class template
// This file contains the implementation of all helper functions

namespace career {

// ============================================================================
// Initialization Helpers
// ============================================================================

template<typename T, typename Allocator>
void Deque<T, Allocator>::default_initialize() {
    using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
    MapPointer cur;
    try {
        for (cur = this->data.start.node; cur < this->data.finish.node; ++cur) {
            pointer p = *cur;
            pointer end = p + this->data.start.buffer_size();
            for (; p != end; ++p) {
                allocator_traits::construct(this->allocator, p);
            }
        }
        // Handle last (partial) buffer
        pointer p = this->data.finish.first;
        for (; p != this->data.finish.current; ++p) {
            allocator_traits::construct(this->allocator, p);
        }
    } catch (...) {
        // Cleanup on exception
        destroy_data(this->data.start, iterator(this->data.finish.first, cur));
        throw;
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::fill_initialize(const T& value) {
    using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
    MapPointer cur;
    try {
        for (cur = this->data.start.node; cur < this->data.finish.node; ++cur) {
            std::uninitialized_fill(*cur, *cur + this->data.start.buffer_size(), value);
        }
        // Handle last (partial) buffer
        std::uninitialized_fill(this->data.finish.first, this->data.finish.current, value);
    } catch (...) {
        // Cleanup on exception
        destroy_data(this->data.start, iterator(this->data.finish.first, cur));
        throw;
    }
}

template<typename T, typename Allocator>
template<typename InputIterator>
void Deque<T, Allocator>::range_initialize(InputIterator first, InputIterator last,
                                           std::input_iterator_tag) {
    try {
        for (; first != last; ++first) {
            emplace_back(*first);
        }
    } catch (...) {
        clear();
        throw;
    }
}

template<typename T, typename Allocator>
template<typename ForwardIterator>
void Deque<T, Allocator>::range_initialize(ForwardIterator first, ForwardIterator last,
                                           std::forward_iterator_tag) {
    using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
    const size_type n = std::distance(first, last);
    this->initialize_map(check_size(n));
    
    MapPointer cur_node;
    try {
        for (cur_node = this->data.start.node; cur_node < this->data.finish.node; ++cur_node) {
            ForwardIterator mid = first;
            std::advance(mid, this->data.start.buffer_size());
            std::uninitialized_copy(first, mid, *cur_node);
            first = mid;
        }
        // Handle last (partial) buffer
        std::uninitialized_copy(first, last, this->data.finish.first);
    } catch (...) {
        destroy_data(this->data.start, iterator(this->data.finish.first, cur_node));
        throw;
    }
}

// ============================================================================
// Assignment Helpers
// ============================================================================

template<typename T, typename Allocator>
void Deque<T, Allocator>::fill_assign(size_type count, const T& value) {
    if (count > size()) {
        std::fill(begin(), end(), value);
        const size_type to_add = count - size();
        insert(end(), to_add, value);
    } else {
        erase_at_end(begin() + count);
        std::fill(begin(), end(), value);
    }
}

template<typename T, typename Allocator>
template<typename InputIterator>
void Deque<T, Allocator>::assign_aux(InputIterator first, InputIterator last,
                                     std::input_iterator_tag) {
    iterator cur = begin();
    for (; first != last && cur != end(); ++first, ++cur) {
        *cur = *first;
    }
    
    if (first == last) {
        erase_at_end(cur);
    } else {
        range_insert_aux(end(), first, last, std::input_iterator_tag{});
    }
}

template<typename T, typename Allocator>
template<typename ForwardIterator>
void Deque<T, Allocator>::assign_aux(ForwardIterator first, ForwardIterator last,
                                     std::forward_iterator_tag) {
    const size_type len = std::distance(first, last);
    
    if (len > size()) {
        ForwardIterator mid = first;
        std::advance(mid, size());
        std::copy(first, mid, begin());
        range_insert_aux(end(), mid, last, std::forward_iterator_tag{});
    } else {
        erase_at_end(std::copy(first, last, begin()));
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::move_assign(Deque&& other, std::true_type) noexcept {
    clear();
    this->data.swap_data(other.data);
    this->allocator = std::move(other.allocator);
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::move_assign(Deque&& other, std::false_type) {
    if (this->allocator == other.allocator) {
        move_assign(std::move(other), std::true_type{});
    } else {
        assign(std::make_move_iterator(other.begin()),
               std::make_move_iterator(other.end()));
    }
}

// ============================================================================
// Push/Pop Auxiliary Functions
// ============================================================================

template<typename T, typename Allocator>
void Deque<T, Allocator>::push_front_aux(const T& value) {
    reserve_map_at_front();
    *(this->data.start.node - 1) = this->allocate_node();
    try {
        this->data.start.set_node(this->data.start.node - 1);
        this->data.start.current = this->data.start.last - 1;
        allocator_traits::construct(this->allocator, this->data.start.current, value);
    } catch (...) {
        ++this->data.start;
        this->deallocate_node(*(this->data.start.node - 1));
        throw;
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::push_back_aux(const T& value) {
    reserve_map_at_back();
    *(this->data.finish.node + 1) = this->allocate_node();
    try {
        allocator_traits::construct(this->allocator, this->data.finish.current, value);
        this->data.finish.set_node(this->data.finish.node + 1);
        this->data.finish.current = this->data.finish.first;
    } catch (...) {
        this->deallocate_node(*(this->data.finish.node + 1));
        throw;
    }
}

template<typename T, typename Allocator>
template<typename... Args>
void Deque<T, Allocator>::emplace_front_aux(Args&&... args) {
    reserve_map_at_front();
    *(this->data.start.node - 1) = this->allocate_node();
    try {
        this->data.start.set_node(this->data.start.node - 1);
        this->data.start.current = this->data.start.last - 1;
        allocator_traits::construct(this->allocator, this->data.start.current, 
                                   std::forward<Args>(args)...);
    } catch (...) {
        ++this->data.start;
        this->deallocate_node(*(this->data.start.node - 1));
        throw;
    }
}

template<typename T, typename Allocator>
template<typename... Args>
void Deque<T, Allocator>::emplace_back_aux(Args&&... args) {
    reserve_map_at_back();
    *(this->data.finish.node + 1) = this->allocate_node();
    try {
        allocator_traits::construct(this->allocator, this->data.finish.current,
                                   std::forward<Args>(args)...);
        this->data.finish.set_node(this->data.finish.node + 1);
        this->data.finish.current = this->data.finish.first;
    } catch (...) {
        this->deallocate_node(*(this->data.finish.node + 1));
        throw;
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::pop_front_aux() {
    allocator_traits::destroy(this->allocator, this->data.start.current);
    this->deallocate_node(this->data.start.first);
    this->data.start.set_node(this->data.start.node + 1);
    this->data.start.current = this->data.start.first;
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::pop_back_aux() {
    this->deallocate_node(this->data.finish.first);
    this->data.finish.set_node(this->data.finish.node - 1);
    this->data.finish.current = this->data.finish.last - 1;
    allocator_traits::destroy(this->allocator, this->data.finish.current);
}

// ============================================================================
// Insert Auxiliary Functions
// ============================================================================

template<typename T, typename Allocator>
template<typename... Args>
typename Deque<T, Allocator>::iterator 
Deque<T, Allocator>::insert_aux(iterator position, Args&&... args) {
    const difference_type index = position - this->data.start;
    
    if (static_cast<size_type>(index) < size() / 2) {
        // Insert by moving elements at front
        emplace_front(front());
        iterator front1 = this->data.start;
        ++front1;
        iterator front2 = front1;
        ++front2;
        position = this->data.start + index;
        iterator pos1 = position;
        ++pos1;
        std::move(front2, pos1, front1);
    } else {
        // Insert by moving elements at back
        emplace_back(back());
        iterator back1 = this->data.finish;
        --back1;
        iterator back2 = back1;
        --back2;
        position = this->data.start + index;
        std::move_backward(position, back2, back1);
    }
    
    *position = T(std::forward<Args>(args)...);
    return position;
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::fill_insert(iterator position, size_type count, const T& value) {
    if (position.current == this->data.start.current) {
        iterator new_start = reserve_elements_at_front(count);
        try {
            std::uninitialized_fill(new_start, this->data.start, value);
            this->data.start = new_start;
        } catch (...) {
            this->destroy_nodes(new_start.node, this->data.start.node);
            throw;
        }
    } else if (position.current == this->data.finish.current) {
        iterator new_finish = reserve_elements_at_back(count);
        try {
            std::uninitialized_fill(this->data.finish, new_finish, value);
            this->data.finish = new_finish;
        } catch (...) {
            this->destroy_nodes(this->data.finish.node + 1, new_finish.node + 1);
            throw;
        }
    } else {
        const size_type elems_before = position - this->data.start;
        const size_type elems_after = this->data.finish - position;
        const size_type old_size = size();
        
        if (elems_before < elems_after) {
            // Shift elements at front
            iterator new_start = reserve_elements_at_front(count);
            iterator old_start = this->data.start;
            position = this->data.start + elems_before;
            
            try {
                if (elems_before >= count) {
                    iterator start_n = this->data.start + count;
                    std::uninitialized_move(this->data.start, start_n, new_start);
                    this->data.start = new_start;
                    std::move(start_n, position, old_start);
                    std::fill(position - count, position, value);
                } else {
                    iterator mid = std::uninitialized_move(this->data.start, position, new_start);
                    try {
                        std::uninitialized_fill(mid, this->data.start, value);
                    } catch (...) {
                        destroy_data(new_start, mid);
                        throw;
                    }
                    this->data.start = new_start;
                    std::fill(old_start, position, value);
                }
            } catch (...) {
                this->destroy_nodes(new_start.node, this->data.start.node);
                throw;
            }
        } else {
            // Shift elements at back
            iterator new_finish = reserve_elements_at_back(count);
            iterator old_finish = this->data.finish;
            const size_type elems_after_n = elems_after - count;
            position = this->data.finish - elems_after;
            
            try {
                if (elems_after > count) {
                    iterator finish_n = this->data.finish - count;
                    std::uninitialized_move(finish_n, this->data.finish, this->data.finish);
                    this->data.finish = new_finish;
                    std::move_backward(position, finish_n, old_finish);
                    std::fill(position, position + count, value);
                } else {
                    std::uninitialized_fill(this->data.finish, position + count, value);
                    try {
                        std::uninitialized_move(position, this->data.finish, position + count);
                    } catch (...) {
                        destroy_data(this->data.finish, position + count);
                        throw;
                    }
                    this->data.finish = new_finish;
                    std::fill(position, old_finish, value);
                }
            } catch (...) {
                this->destroy_nodes(this->data.finish.node + 1, new_finish.node + 1);
                throw;
            }
        }
    }
}

template<typename T, typename Allocator>
template<typename InputIterator>
void Deque<T, Allocator>::range_insert_aux(iterator position, 
                                           InputIterator first, InputIterator last,
                                           std::input_iterator_tag) {
    std::copy(first, last, std::inserter(*this, position));
}

template<typename T, typename Allocator>
template<typename ForwardIterator>
void Deque<T, Allocator>::range_insert_aux(iterator position,
                                           ForwardIterator first, ForwardIterator last,
                                           std::forward_iterator_tag) {
    const size_type count = std::distance(first, last);
    if (count == 0) return;
    
    if (position.current == this->data.start.current) {
        iterator new_start = reserve_elements_at_front(count);
        try {
            std::uninitialized_copy(first, last, new_start);
            this->data.start = new_start;
        } catch (...) {
            this->destroy_nodes(new_start.node, this->data.start.node);
            throw;
        }
    } else if (position.current == this->data.finish.current) {
        iterator new_finish = reserve_elements_at_back(count);
        try {
            std::uninitialized_copy(first, last, this->data.finish);
            this->data.finish = new_finish;
        } catch (...) {
            this->destroy_nodes(this->data.finish.node + 1, new_finish.node + 1);
            throw;
        }
    } else {
        const size_type elems_before = position - this->data.start;
        
        if (elems_before < size() / 2) {
            // Shift elements at front
            iterator new_start = reserve_elements_at_front(count);
            iterator old_start = this->data.start;
            position = this->data.start + elems_before;
            
            try {
                if (elems_before >= count) {
                    iterator start_n = this->data.start + count;
                    std::uninitialized_move(this->data.start, start_n, new_start);
                    this->data.start = new_start;
                    std::move(start_n, position, old_start);
                    std::copy(first, last, position - count);
                } else {
                    ForwardIterator mid = first;
                    std::advance(mid, elems_before);
                    iterator new_mid = std::uninitialized_copy(first, mid, new_start);
                    try {
                        std::uninitialized_move(this->data.start, position, new_mid);
                    } catch (...) {
                        destroy_data(new_start, new_mid);
                        throw;
                    }
                    this->data.start = new_start;
                    std::copy(mid, last, old_start);
                }
            } catch (...) {
                this->destroy_nodes(new_start.node, this->data.start.node);
                throw;
            }
        } else {
            // Shift elements at back
            iterator new_finish = reserve_elements_at_back(count);
            iterator old_finish = this->data.finish;
            const size_type elems_after = this->data.finish - position;
            position = this->data.finish - elems_after;
            
            try {
                if (elems_after > count) {
                    iterator finish_n = this->data.finish - count;
                    std::uninitialized_move(finish_n, this->data.finish, this->data.finish);
                    this->data.finish = new_finish;
                    std::move_backward(position, finish_n, old_finish);
                    std::copy(first, last, position);
                } else {
                    ForwardIterator mid = first;
                    std::advance(mid, elems_after);
                    std::uninitialized_copy(mid, last, this->data.finish);
                    try {
                        std::uninitialized_move(position, this->data.finish, 
                                              this->data.finish + (count - elems_after));
                    } catch (...) {
                        destroy_data(this->data.finish, this->data.finish + (count - elems_after));
                        throw;
                    }
                    this->data.finish = new_finish;
                    std::copy(first, mid, position);
                }
            } catch (...) {
                this->destroy_nodes(this->data.finish.node + 1, new_finish.node + 1);
                throw;
            }
        }
    }
}

// ============================================================================
// Erase Auxiliary Functions
// ============================================================================

template<typename T, typename Allocator>
typename Deque<T, Allocator>::iterator 
Deque<T, Allocator>::erase_aux(iterator position) {
    iterator next = position;
    ++next;
    const difference_type index = position - this->data.start;
    
    if (static_cast<size_type>(index) < size() / 2) {
        // Move elements from front
        std::move_backward(this->data.start, position, next);
        pop_front();
    } else {
        // Move elements from back
        std::move(next, this->data.finish, position);
        pop_back();
    }
    
    return this->data.start + index;
}

template<typename T, typename Allocator>
typename Deque<T, Allocator>::iterator 
Deque<T, Allocator>::erase_aux(iterator first, iterator last) {
    if (first == this->data.start && last == this->data.finish) {
        clear();
        return this->data.finish;
    }
    
    const difference_type n = last - first;
    const difference_type elems_before = first - this->data.start;
    
    if (static_cast<size_type>(elems_before) < (size() - n) / 2) {
        // Move elements from front
        using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
        std::move_backward(this->data.start, first, last);
        iterator new_start = this->data.start + n;
        destroy_data(this->data.start, new_start);
        
        // Deallocate buffers
        for (MapPointer node = this->data.start.node; node < new_start.node; ++node) {
            this->deallocate_node(*node);
        }
        
        this->data.start = new_start;
    } else {
        // Move elements from back
        using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
        std::move(last, this->data.finish, first);
        iterator new_finish = this->data.finish - n;
        destroy_data(new_finish, this->data.finish);
        
        // Deallocate buffers
        for (MapPointer node = new_finish.node + 1; node <= this->data.finish.node; ++node) {
            this->deallocate_node(*node);
        }
        
        this->data.finish = new_finish;
    }
    
    return this->data.start + elems_before;
}

// ============================================================================
// Resize Helpers
// ============================================================================

template<typename T, typename Allocator>
void Deque<T, Allocator>::default_append(size_type count) {
    if (count == 0) return;
    
    iterator new_finish = reserve_elements_at_back(count);
    try {
        for (iterator it = this->data.finish; it != new_finish; ++it) {
            allocator_traits::construct(this->allocator, it.current);
        }
        this->data.finish = new_finish;
    } catch (...) {
        destroy_data(this->data.finish, new_finish);
        this->destroy_nodes(this->data.finish.node + 1, new_finish.node + 1);
        throw;
    }
}

// ============================================================================
// Cleanup Helpers
// ============================================================================

template<typename T, typename Allocator>
void Deque<T, Allocator>::destroy_data(iterator first, iterator last) {
    using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
    for (MapPointer node = first.node + 1; node < last.node; ++node) {
        for (pointer p = *node; p != *node + this->data.start.buffer_size(); ++p) {
            allocator_traits::destroy(this->allocator, p);
        }
    }
    
    if (first.node != last.node) {
        for (pointer p = first.current; p != first.last; ++p) {
            allocator_traits::destroy(this->allocator, p);
        }
        for (pointer p = last.first; p != last.current; ++p) {
            allocator_traits::destroy(this->allocator, p);
        }
    } else {
        for (pointer p = first.current; p != last.current; ++p) {
            allocator_traits::destroy(this->allocator, p);
        }
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::erase_at_end(iterator position) {
    using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
    destroy_data(position, this->data.finish);
    
    // Deallocate unused buffers
    for (MapPointer node = position.node + 1; node <= this->data.finish.node; ++node) {
        this->deallocate_node(*node);
    }
    
    this->data.finish = position;
}

// ============================================================================
// Memory Management Helpers
// ============================================================================

template<typename T, typename Allocator>
typename Deque<T, Allocator>::iterator
Deque<T, Allocator>::reserve_elements_at_front(size_type n) {
    const size_type vacancies = this->data.start.current - this->data.start.first;
    if (n > vacancies) {
        new_elements_at_front(n - vacancies);
    }
    return this->data.start - difference_type(n);
}

template<typename T, typename Allocator>
typename Deque<T, Allocator>::iterator
Deque<T, Allocator>::reserve_elements_at_back(size_type n) {
    const size_type vacancies = (this->data.finish.last - this->data.finish.current) - 1;
    if (n > vacancies) {
        new_elements_at_back(n - vacancies);
    }
    return this->data.finish + difference_type(n);
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::new_elements_at_front(size_type new_elems) {
    const size_type new_nodes = (new_elems + this->data.start.buffer_size() - 1) / 
                                this->data.start.buffer_size();
    reserve_map_at_front(new_nodes);
    
    size_type i;
    try {
        for (i = 1; i <= new_nodes; ++i) {
            *(this->data.start.node - i) = this->allocate_node();
        }
    } catch (...) {
        for (size_type j = 1; j < i; ++j) {
            this->deallocate_node(*(this->data.start.node - j));
        }
        throw;
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::new_elements_at_back(size_type new_elems) {
    const size_type new_nodes = (new_elems + this->data.start.buffer_size() - 1) / 
                                this->data.start.buffer_size();
    reserve_map_at_back(new_nodes);
    
    size_type i;
    try {
        for (i = 1; i <= new_nodes; ++i) {
            *(this->data.finish.node + i) = this->allocate_node();
        }
    } catch (...) {
        for (size_type j = 1; j < i; ++j) {
            this->deallocate_node(*(this->data.finish.node + j));
        }
        throw;
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::reserve_map_at_back(size_type nodes_to_add) {
    if (nodes_to_add + 1 > this->data.map_size - (this->data.finish.node - this->data.map)) {
        reallocate_map(nodes_to_add, false);
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::reserve_map_at_front(size_type nodes_to_add) {
    if (nodes_to_add > size_type(this->data.start.node - this->data.map)) {
        reallocate_map(nodes_to_add, true);
    }
}

template<typename T, typename Allocator>
void Deque<T, Allocator>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
    using MapPointer = typename DequeBase<T, Allocator>::MapPointer;
    const size_type old_num_nodes = this->data.finish.node - this->data.start.node + 1;
    const size_type new_num_nodes = old_num_nodes + nodes_to_add;
    
    MapPointer new_nstart;
    if (this->data.map_size > 2 * new_num_nodes) {
        // We have room in the existing map, just reposition
        new_nstart = this->data.map + (this->data.map_size - new_num_nodes) / 2
                     + (add_at_front ? nodes_to_add : 0);
        
        if (new_nstart < this->data.start.node) {
            std::copy(this->data.start.node, this->data.finish.node + 1, new_nstart);
        } else {
            std::copy_backward(this->data.start.node, this->data.finish.node + 1,
                             new_nstart + old_num_nodes);
        }
    } else {
        // Need to allocate a new map
        size_type new_map_size = this->data.map_size + 
                                std::max(this->data.map_size, nodes_to_add) + 2;
        
        MapPointer new_map = this->allocate_map(new_map_size);
        new_nstart = new_map + (new_map_size - new_num_nodes) / 2
                     + (add_at_front ? nodes_to_add : 0);
        
        std::copy(this->data.start.node, this->data.finish.node + 1, new_nstart);
        this->deallocate_map(this->data.map, this->data.map_size);
        
        this->data.map = new_map;
        this->data.map_size = new_map_size;
    }
    
    this->data.start.set_node(new_nstart);
    this->data.finish.set_node(new_nstart + old_num_nodes - 1);
}

} // namespace career
