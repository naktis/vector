# Vector
Custom C++ vector class, written by recreating *std::vector* container

## Installation and usage:

 1. Download the header file from the [releases page](https://github.com/Naktis/vector/releases) or using *git*:
 
    `git clone https://github.com/Naktis/vector.git`
 2. Include the header file to your program
 
    i.e.: If the header file is in the same directory, write `#include "vector.hpp"` at the top of your file
 3. Use it the same way as you would use *std::vector*, but ignore the *std::* part when declaring new vectors

## Some of the functions
 1. **Assign** - replaces the contents with *count* copies of *value*
 
    ```cpp
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
    ```
 2. **Erase** - removes the element at *pos* position
 
    ```cpp
    template <class T> 
    typename vector<T>::iterator vector<T>::erase( iterator pos ) {
        if (pos < data || pos >= avail)
            throw std::out_of_range{ "vector::erase" };

        for (iterator i = pos; i < avail-1; i ++)
            *i = *(i+1); // Move values by one position to the left

        avail --;        // Reduce the vector size by one
        return pos;
    }
    ```
 3. **Swap** - exchanges the contents of the container with those of other. Does not invoke any move, copy, or swap operations on individual elements
 
    ```cpp
    template <class T>
    void vector<T>::swap( vector<T>& other ) {
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
    ```
 4. **Operator ==** - checks if both vectors have the same size and values
 
    ```cpp
    template <class T>cpp
    bool vector<T>::operator==(const vector<T>& other) const {
        if (size() == other.size()) {
            for (int i = 0; i < size(); i ++)
                if (at(i) != other.at(i))   // Search for any mismatch
                    return false;           // Break the loop, return immediate result
            return true;
        } else return false;
    }
    ```
 5. **Operator <** - compares the contents of two vectors lexicographically: checks elements each by each; if the operator finds a mismatching element, it returns their comparison result. If one range is a prefix of another, the shorter range is lexicographically less than the other (i.e. "Bana" < "Banana", because "Bana" is a prefix of "Banana" and they have no mismatching elements)
 
    ```cpp
    template <class T>cpp
    bool vector<T>::operator<(const vector<T>& other) const {
        size_type smaller_size;
        if (size() < other.size())
            smaller_size = size();
        else smaller_size = other.size();

        for (size_type i = 0; i < smaller_size; i++)
            if (at(i) != other[i])       // Find the first mismatching element
                return at(i) < other[i];

        return size() < other.size();
    }
    ```

### Runtime analysis // Adding new integers with .push_back()
Hardware spec: Intel i7 CPU @ 1.80 GHz, 16 GB DDR3 RAM

| Int count | std::vector | vector    |
| --------- | ----------- | --------- |
| 10000     | 0.000924s   | 0.000332s |
| 100000    | 0.003422s   | 0.002893s |
| 1000000   | 0.032313s   | 0.030324s |
| 10000000  | 0.272299s   | 0.233536s |
| 100000000 | 2.11631s    | 2.22146s  |

In almost all cases *vector* container slightly outperformed *std::vector* container. With 100M integers *std::vector* was quicker than *vector*.

### Reallocation count analysis // Adding new integers with .push_back()

Reallocation occurs when size() reaches the same value as capacity()

Element count: 100 000 000 integers

| Container   | Reallocation count | Reached capacity |
| ----------- | ------------------ | ---------------- |
| std::vector | 28                 | 134217728        |
| vector      | 28                 | 134217728        |

Using *vector* container the occupied memory was reallocated the same number of times as using *std::vector*.


### Runtime analysis // [Final grades 2](https://github.com/Naktis/final-grades-2)

Student count: 100 000

| Container   | Runtime  |
| ----------- | -------- |
| std::vector | 2.28704s |
| vector      | 2.04709s |

Replacing *std::vector* with *vector* container made the program run a little faster. It works just as fine as it did with *std::vector* container, final grades calculation results and output file remained the same.

### Testing

Most of the constructors, functions and operators were tested with [Catch](https://github.com/catchorg/Catch2). Returned values, adjusted container size and capacity values were compared with results of similar *std::vector* functions. All [unit tests](tests/functions.cpp) passed successfully.

![tests](https://i.imgur.com/nOrFf0a.jpg)

To run tests by yourself, download a c++ compiler (i.e. [GCC](https://gcc.gnu.org/)), go to the *vector/tests* directory, type `g++ functions.cpp -o fun` and run the code with `./fun` (unix) or `fun` (windows)
