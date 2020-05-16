#pragma once

#include <memory>

template <class T> 
class Vector {
    public:
        // Member types
        typedef T value_type;
        typedef std::allocator<T> allocator_type;
        typedef size_t size_type;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        //typedef std::allocator_traits<alloc>::pointer pointer;
        //typedef std::allocator_traits<alloc>::const_pointer const_pointer;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        // Member functions
        Vector() { create(); }
        explicit Vector(size_type n, const T& val = T{}) { create(n, val); }
        Vector(const Vector& v) { create(v.begin(), v.end()); } // copy constructor
        ~Vector() { uncreate(); }           // destructor
        Vector& operator=(const Vector&);   // copy assignment operator
        void assign( size_type count, const T& value );
        allocator_type get_allocator() const { return alloc; };

        // Element access
        reference at( size_type i ) { if (i < size()) return data[i]; else throw std::out_of_range {"Vector::at"};}
        const_reference at( size_type i ) const { if (i < size()) return data[i]; else throw std::out_of_range {"Vector::at"};}
        T& operator[](size_type i) { return data[i]; }
        const T& operator[](size_type i) const { return data[i]; }
        reference front() { return *data; }
        const_reference front() const { return *data; }
        reference back() { return *(end()-1); }
        const_reference back() const { return *(end()-1); }

        // Iterators
        iterator begin(){ return data; }
        const_iterator begin()const{ return data; }
        iterator end(){ return avail; }
        const_iterator end()const{ return avail; }

        // Capacity
        bool empty() const { return (begin() == end()); }
        size_type size() const { return avail - data; }
        size_type capacity() const { return limit - data; }
        //size_type max_size() const {return std::distance(begin(), end()); } // TODO: pratestuoti
        void reserve( size_type new_cap ); // REIKIA APSAUGOS AGIRDI
        void shrink_to_fit();

        // Modifiers
        void clear();
        iterator insert( const_iterator pos, const T& value );
        iterator erase( iterator pos );
        iterator erase( iterator first, iterator last );
        void push_back(const T& val);
        void pop_back();
        void resize( size_type count );
        void resize( size_type count, const value_type& value );
    private:    
        iterator data;        // pirmasis Vector elementas    
        iterator avail;       // pirmasis po paskutiniojo sukonstruoto Vector elementas
        iterator limit;       // pirmasis po paskutiniojo Vector elementas

        allocator_type alloc; // atminties išskyrimo valdymui    
           // objektas atminties valdymui
        // išskirti atmintį (array) ir ją inicializuoti
        void create();
        void create(size_type, const T&);
        void create(const_iterator, const_iterator);
        // sunaikinti elementus array ir atlaisvinti atmintį
        void uncreate();
        // pagalbinės funkcijos push_back realizacijai
        void growTwice();
        void unchecked_append(const T&);
};

// priskyrimo operatoriaus realizacija
template <class T> 
Vector<T>& Vector<T>::operator=(const Vector& rhs) {
    // patikriname ar ne lygu
    if (&rhs != this) {
        // atlaisviname seną this objekto atmintį        
        uncreate();
        // perkopijuojame elementus iš rhs į lhs (this)        
        create(rhs.begin(), rhs.end());    
    }
    return *this;
}

template <class T> 
void Vector<T>::create() {
    data = avail = limit = nullptr;
}

template <class T> 
void Vector<T>::create(size_type n, const T& val) {
    data = alloc.allocate(n); // grąžina ptr į array pirmą elementą    
    limit = avail = data + n; // sustato rodykles į vietas    
    std::uninitialized_fill(data, limit, val); // inicializuoja elementus val reikšme
}

template <class T>
void Vector<T>::create(const_iterator i, const_iterator j) {    
    data = alloc.allocate(j - i); // išskirti vietos j-i elementams    
    limit = avail = std::uninitialized_copy(i, j, data); // nukopijuoja elementus iš intervalo 
}

template <class T> 
void Vector<T>::uncreate() {
    if (data) {
        // sunaikinti (atbuline tvarka) sukonstruotus elementus        
        iterator it = avail;
        while (it != data)            
            alloc.destroy(--it);
            // atlaisvinti išskirtą atmintį. Turi būti data != nullptr        
            alloc.deallocate(data, limit - data);    
        }
    // reset'inam pointer'iuss - Vector'ius tuščias    
    data =  limit  =  avail  =  nullptr;
}

template <class T> 
void Vector<T>::growTwice() {
    // dvigubai daugiau vietos, nei buvo    
    size_type new_size = std::max(2 * (limit - data), ptrdiff_t(1));
    // išskirti naują vietą ir perkopijuoti egzistuojančius elementus    
    reserve(new_size);
}

// tariame, kad `avail` point'ina į išskirtą, bet neinicializuotą vietą
template <class T> 
void Vector<T>::unchecked_append(const T& val) {    
    alloc.construct(avail++, val);
}

template <class T> 
void Vector<T>::reserve( size_type new_cap ) {
    if (new_cap > capacity()) {
        iterator new_data = alloc.allocate(new_cap);    
        iterator new_avail = std::uninitialized_copy(data, avail, new_data);// atlaisvinti seną vietą    
        uncreate();
        // reset'int rodykles į naujai išskirtą vietą    
        data = new_data;    
        avail = new_avail;     
        limit = data + new_cap;
    }
}

template <class T> 
void Vector<T>::shrink_to_fit() {
    if (avail != limit) {
        alloc.deallocate(avail, limit - avail);
        limit = avail;
    }
}

template <class T> 
void Vector<T>::assign( size_type count, const T& value ) {
    if (count > capacity()) {
        uncreate();
        create(count, value);
    } else {
        iterator it = avail;
        while (it != data)            
            alloc.destroy(--it);
        avail = data + count; // sustato rodykles į vietas    
        std::uninitialized_fill(data, avail, value); // inicializuoja elementus val reikšme
    }
}

template <class T> 
void Vector<T>::clear() { 
    iterator it = avail;
    while (it != data)
        alloc.destroy(--it);
    avail = data;
}

template <class T> 
typename Vector<T>::iterator Vector<T>::insert( const_iterator pos, const T& value ) {
    int pos_integer = 0;
    for (iterator i = data; i < pos; i++)
        pos_integer ++;

    size_type new_size = (size() + 1);
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

    return avail;
}

template <class T> 
typename Vector<T>::iterator Vector<T>::erase( iterator pos ) {
    for (iterator i = pos; i < avail-1; i ++)
        *i = *(i+1);
    avail --;
    return pos;
}

template <class T> 
typename Vector<T>::iterator Vector<T>::erase( iterator first, iterator last ) {
    for (iterator i = first; i < avail-1; i ++)
        *i = *(i+(last-first));
    avail -= last - first;
    return last;    
}

template <class T> 
void Vector<T>::push_back(const T& val){
    if (avail == limit)        
        growTwice();        // Increase the container capacity twice      
    unchecked_append(val) ; // Insert new element
}

template <class T> 
void Vector<T>::pop_back(){
    iterator it = avail;           
    alloc.destroy(--it);
    avail--;
}

template <class T> 
void Vector<T>::resize( size_type count ) {
    while (begin() + count != avail)
        alloc.destroy(--avail);
}

template <class T>
void Vector<T>::resize( size_type count, const value_type& value ) {
    if (size() > count)
        resize(count);
    else {
        if (limit == avail)
            reserve(size() + (count - size()));
        std::uninitialized_fill(avail, limit, value);
        avail = limit;
    }
}