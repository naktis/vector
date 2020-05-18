#pragma once

#include <memory>

template <class T> 
class vector {
    public:
        // Member types
        typedef T value_type;
        typedef std::allocator<T> allocator_type;
        typedef size_t size_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        // Member functions
        vector() { create(); }
        explicit vector(size_type n, const T& val = T{}) { create(n, val); }
        vector(std::initializer_list<T> array); // Create from array
        vector(const vector& v) { create(v.begin(), v.end()); } // copy
        ~vector() { uncreate(); }
        vector& operator=(const vector&);       // copy assignment
        void assign( size_type count, const T& value );
        allocator_type get_allocator() const { return alloc; };

        // Element access
        reference at( size_type i );
        const_reference at( size_type i ) const;
        T& operator[](size_type i) { return data[i]; }
        const T& operator[](size_type i) const { return data[i]; }
        reference front() { return *data; }
        const_reference front() const { return *data; }
        reference back() { return *(avail-1); }
        const_reference back() const { return *(avail-1); }

        // Iterators
        iterator begin() noexcept { return data; }
        const_iterator begin() const noexcept { return data; }
        iterator end() noexcept { return avail; }
        const_iterator end() const noexcept { return avail; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(avail); }
        const_reverse_iterator rbegin() const noexcept { return reverse_iterator(avail); };
        reverse_iterator rend() noexcept { return reverse_iterator(data); }
        const_reverse_iterator rend() const noexcept { return reverse_iterator(data); };

        // Capacity
        bool empty() const { return (begin() == end()); }
        size_type size() const { return avail - data; }
        size_type capacity() const { return limit - data; }
        void reserve( size_type new_cap );
        void shrink_to_fit();

        // Modifiers
        void clear();
        iterator insert( const_iterator pos, const T& value );
        iterator insert( const_iterator pos, int count, T& value);
        iterator erase( iterator pos );
        iterator erase( iterator first, iterator last );
        void push_back(const T& val);
        void pop_back();
        void resize( size_type count );
        void resize( size_type count, const value_type& value );
        void swap( vector<T>& other ) noexcept;

        // Operators
        bool operator==(const vector<T>& other) const;
        bool operator!=(const vector<T>& other) const;
        bool operator<(const vector<T>& other) const;
        bool operator>(const vector<T>& other) const;
        bool operator>=(const vector<T>& other) const;
        bool operator<=(const vector<T>& other) const;
    private:    
        iterator data;        // first element of the vector
        iterator avail;       // first element after the last vector element
        iterator limit;       // first element outside the reserved space

        allocator_type alloc;

        void create();        // set pointers to null
        void create(size_type, const T&);   // create a vec with copies of 'value'
        void create(const_iterator, const_iterator); // create a vec with contents of a range
        void uncreate();      // destroy the vec and deallocate space
        void growTwice();     // increase the reserved space twice
        void unchecked_append(const T&);    // Insert new element at the end
};

// Create a vector from an array
template <class T> 
vector<T>::vector(std::initializer_list<T> array) {
    data = alloc.allocate(array.size());
    limit = avail = std::uninitialized_copy(array.begin(), array.end(), data);
}

// Create an empty vector
template <class T> 
void vector<T>::create() {
    data = avail = limit = nullptr;
}

// Create a vector with copies of 'value' or just reserved space
template <class T> 
void vector<T>::create(size_type n, const T& val) {
    data = alloc.allocate(n); 
    limit = avail = data + n;
    std::uninitialized_fill(data, limit, val);
}

// Create a vector with contents of a range
template <class T>
void vector<T>::create(const_iterator i, const_iterator j) {    
    data = alloc.allocate(j - i);
    limit = avail = std::uninitialized_copy(i, j, data);
}

// Destroy the vector and deallocate space
template <class T> 
void vector<T>::uncreate() {
    if (data) {
        // Destroy elements backwards     
        iterator it = avail;
        while (it != data)
            alloc.destroy(--it);      
        alloc.deallocate(data, limit - data);    
        }
    data = limit = avail = nullptr; // Reset pointers
}

// Assignment operator
template <class T> 
vector<T>& vector<T>::operator=(const vector& rhs) {
    if (&rhs != this) {     
        uncreate();
        create(rhs.begin(), rhs.end());
    }
    return *this;
}

// Increase the reserved space twice
template <class T> 
void vector<T>::growTwice() { 
    size_type new_size = std::max((limit - data) * 2, ptrdiff_t(1));   
    reserve(new_size);
}

// Insert new element at the end
template <class T> 
void vector<T>::unchecked_append(const T& val) {    
    alloc.construct(avail++, val);
}

// Element access funtions
template <class T>
typename vector<T>::reference vector<T>::at( size_type i ) {
    if (i < size() && i >= 0)
        return data[i]; 
    else throw std::out_of_range {"vector::at"};
}

template <class T>
typename vector<T>::const_reference vector<T>::at( size_type i ) const { 
    if (i < size() && i >= 0) 
        return data[i]; 
    else throw std::out_of_range {"vector::at"};
}

// Reallocate vector to a larger block of memory
template <class T> 
void vector<T>::reserve( size_type new_cap ) {
    if (new_cap > capacity()) {
        iterator new_data = alloc.allocate(new_cap);    
        iterator new_avail = std::uninitialized_copy(data, avail, new_data);   
        uncreate();
        data = new_data;    
        avail = new_avail;     
        limit = data + new_cap;
    }
}

// Release unused memory by the vector
template <class T> 
void vector<T>::shrink_to_fit() {
    if (avail != limit) {
        alloc.deallocate(avail, limit - avail);
        limit = avail;
    }
}

// Replace the contents with *count* copies of *value* value
template <class T> 
void vector<T>::assign( size_type count, const T& value ) {
    if (count < 1)
        throw std::invalid_argument{ "vector::assign" };

    if (count > capacity()) {
        uncreate();             // Erase all elements and deallocate memory
        create(count, value);   // Create a new vector and fill it in
    } else {
        iterator it = avail;
        while (it != data)            
            alloc.destroy(--it);// Erase all elements
        
        avail = data + count;   // Set new vector size
        std::uninitialized_fill(data, avail, value); // Fill the vector with copies
    }
}

// Delete all elements, don't deallocate space
template <class T> 
void vector<T>::clear() { 
    iterator it = avail;
    while (it != data)
        alloc.destroy(--it);
    avail = data;
}

// Insert *value* at *pos* position
template <class T> 
typename vector<T>::iterator vector<T>::insert( const_iterator pos, const T& value ) {
    if (pos < data || pos >= avail)
        throw std::out_of_range{ "vector::insert" };
    int pos_integer = 0;
    for (iterator i = data; i < pos; i++)
        pos_integer ++;

    size_type new_size = size() + 1;
    iterator new_data = alloc.allocate(new_size);
    iterator new_avail = std::uninitialized_copy(data, avail + 1, new_data);

    new_data[pos_integer] = value;
    int after_pos = pos_integer + 1;
    int new_last = size() + 1;

    for (int i = after_pos; i < new_last; i++)
        new_data[i] = data[i - 1];
    
    uncreate();
    data = new_data;
    avail = new_avail;
    limit = data + new_size;

    return data + pos_integer;
}

// Insert *count* copies of *value* at *pos* position
template <class T> 
typename vector<T>::iterator vector<T>::insert( const_iterator pos, int count, T& value )
{
    if (pos < data || count < 1 || pos >= avail)
        throw std::out_of_range{ "vector::insert" };
    avail += count;
    int pos_integer = 0;
    for (iterator i = data; i < pos; i++)
        pos_integer ++;
    size_type new_size = size() + count;
    iterator new_data = alloc.allocate(new_size);
    iterator new_avail = std::uninitialized_copy(data, avail, new_data);

    for (int i = 0; i < pos_integer; i++)
        new_data[i] = data[i];

    for (int i = pos_integer; i <= pos_integer + count; i++)
        new_data[i] = value;

    int after_inserted = pos_integer + count;
    int new_last = size() + count;
    for (int i = after_inserted; i < new_last; i++) {
        new_data[i] = data[i - count];
    }
    uncreate();
    data = new_data;
    avail = new_avail;
    limit = data + new_size;

    return data + pos_integer;
}

// Erase element at pos position
template <class T> 
typename vector<T>::iterator vector<T>::erase( iterator pos ) {
    if (pos < data || pos >= avail)
        throw std::out_of_range{ "vector::erase" };

    for (iterator i = pos; i < avail-1; i ++)
        *i = *(i+1); // Move values by one position to the left

    avail --;        // Reduce the vector size by one
    return pos;
}

// Erase elements in a range
template <class T> 
typename vector<T>::iterator vector<T>::erase( iterator first, iterator last ) {
    if (last < first)
        throw std::invalid_argument{ "vector::erase" };
    if (first < data || last > avail)
        throw std::out_of_range{ "vector::erase" };
    for (iterator i = first; i < avail-1; i ++)
        *i = *(i+(last-first));
    avail -= last - first;
    return last;    
}

// Add an element to the back of the vector
template <class T> 
void vector<T>::push_back(const T& val){
    if (avail == limit)        
        growTwice();        // Increase the container capacity twice      
    unchecked_append(val) ; // Insert new element at the end
}

// Delete the last element of the vector
template <class T> 
void vector<T>::pop_back(){
    iterator it = avail;           
    alloc.destroy(--it);
    avail--;
}

// Leave the vector with *count* elements only (count<size)
template <class T> 
void vector<T>::resize( size_type count ) {
    if (count < 0 || count > size())
        throw std::invalid_argument{ "vector::resize" };
    while (begin() + count != avail)
        alloc.destroy(--avail);
}

// If the current size is less than count, additional elements are 
// appended and initialized with copies of value
template <class T>
void vector<T>::resize( size_type count, const value_type& value ) {
    if (count < 0)
        throw std::invalid_argument{ "vector::resize" };
    if (size() > count)
        resize(count);
    else {
        if (limit == avail)
            reserve(size() + (count - size()));
        std::uninitialized_fill(avail, limit, value);
        avail = limit;
    }
}

// Exchange the contents of the container with those of other
template <class T>
void vector<T>::swap( vector<T>& other ) noexcept {
    iterator temp = data;
    data = other.data;
    other.data = temp;

    temp = avail;
    avail = other.avail;
    other.avail = temp;

    temp = limit;
    limit = other.limit;
    other.limit = temp;
}

// Check if both vectors have the same size and values
template <class T>
bool vector<T>::operator==(const vector<T>& other) const {
    if (size() == other.size()) {
        for (int i = 0; i < size(); i ++)
            if (at(i) != other.at(i))   // Search for any mismatch
                return false;           // Return breaks the loop
        return true;
    } else return false;
}

template <class T>
bool vector<T>::operator!=(const vector<T>& other) const {
    // Use the already implemented == operator
    return *this == other ? false : true;
}

// Compare vectors lexicographically
template <class T>
bool vector<T>::operator<(const vector<T>& other) const {
    size_type smaller_size;
    if (size() < other.size())
        smaller_size = size();
    else smaller_size = other.size();

    for (size_type i = 0; i < smaller_size; i++)
        if (at(i) != other[i])       // Find the first mismatching element
            return at(i) < other[i];
    // If one range is a prefix of another, the shorter range is 
    // lexicographically less than the other
    return size() < other.size();
}

template <class T>
bool vector<T>::operator>(const vector<T>& other) const {
    size_type smaller_size;
    if (size() < other.size())
        smaller_size = size();
    else smaller_size = other.size();

    for (size_type i = 0; i < smaller_size; i++)
        if (at(i) != other[i])
            return at(i) > other[i];
    return size() > other.size();
}

template <class T>
bool vector<T>::operator<=(const vector<T>& other) const {
    // Use the already implemented > operator
    return *this > other ? false : true;
}

template <class T>
bool vector<T>::operator>=(const vector<T>& other) const {
    // Use the already implemented < operator
    return *this < other ? false : true;
}