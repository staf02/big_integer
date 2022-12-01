#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

struct vector {
    using iterator = uint32_t*;
    using const_iterator = uint32_t const*;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector(); // O(N) nothrow

    uint32_t& operator[](size_t i);             // O(1) nothrow
    uint32_t const& operator[](size_t i) const; // O(1) nothrow

    uint32_t* data();             // O(1) nothrow
    uint32_t const* data() const; // O(1) nothrow
    size_t size() const;   // O(1) nothrow

    uint32_t& front();             // O(1) nothrow
    uint32_t const& front() const; // O(1) nothrow

    uint32_t& back();                // O(1) nothrow
    uint32_t const& back() const;    // O(1) nothrow
    void push_back(uint32_t const&); // O(1)* strong
    void pop_back();          // O(1) nothrow

    bool empty() const; // O(1) nothrow

    size_t capacity() const; // O(1) nothrow
    void reserve(size_t);    // O(N) strong
    void shrink_to_fit();    // O(N) strong

    void clear(); // O(N) nothrow

    void swap(vector&); // O(1) nothrow

    iterator begin(); // O(1) nothrow
    iterator end();   // O(1) nothrow

    const_iterator begin() const; // O(1) nothrow
    const_iterator end() const;   // O(1) nothrow

    iterator insert(const_iterator pos, uint32_t const&); // O(N) strong

    iterator erase(const_iterator pos); // O(N) nothrow(swap)

    iterator erase(const_iterator first, const_iterator last); // O(N) nothrow(swap)

    void resize(size_t new_size);

    friend bool operator==(vector const& a, vector const& b);

    void reverse();

private:

    void clear_array(uint32_t* data_array, size_t len) {
        for (size_t i = len; i > 0; i--) {
            data_array[i - 1].~uint32_t();
        }
    }

    void copy_data_array(uint32_t* source, uint32_t*& result, size_t& source_size, size_t& result_size) {
        if (result_size == 0) {
            return;
        }
        result = static_cast<uint32_t*>(operator new(result_size * sizeof(uint32_t)));
        for (size_t i = 0; i < source_size; i++) {
            try {
                new (result + i) uint32_t(source[i]);
            }
            catch (...) {
                clear_array(result, i);
                operator delete(result);
                throw;
            }
        }
    }

    void assign_data_(uint32_t*& new_data, size_t new_capacity) {
        clear_array(data_, size_);
        operator delete(data_);
        data_ = new_data;
        capacity_ = new_capacity;
    }


private:
    uint32_t* data_;
    size_t size_;
    size_t capacity_;
};

