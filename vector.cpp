//
// Created by staf02 on 30/04/22.
//

#include "vector.h"

vector::vector(): data_(nullptr), size_(0), capacity_(0) {}

vector::vector(vector const &other) {
  if (other.capacity_ == 0) {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
  }
  else {
    capacity_ = other.size_;
    size_ = other.size_;
    copy_data_array(other.data_, data_, size_, size_);
  }
}


vector& vector::operator=(vector const& other) {
  vector(other).swap(*this);
  return *this;
}

vector::~vector() {
  clear();
  operator delete(data_);
}

uint32_t& vector::operator[](size_t i) {
  return *(data_ + i);
}

uint32_t const& vector::operator[](size_t i) const {
  return *(data_ + i);
}

uint32_t* vector::data() {
  return data_;
}

uint32_t const* vector::data() const {
  return data_;
}

size_t vector::size() const {
  return size_;
}

uint32_t& vector::front() {
  return data_[0];
}

uint32_t const& vector::front() const {
  return data_[0];
}

uint32_t& vector::back() {
  return data_[size_ - 1];
}

uint32_t const& vector::back() const {
  return data_[size_ - 1];
}

void vector::push_back(uint32_t const& value) {
  if (size_ == capacity_) {
    size_t new_cap = capacity_ == 0 ? 1 : capacity_ * 2;
    uint32_t* new_data = nullptr;
    copy_data_array(data_, new_data, size_, new_cap);
    try {
      new (new_data + size_) uint32_t(value);
    } catch (...) {
      clear_array(new_data, size_);
      operator delete(new_data);
      throw;
    }
    assign_data_(new_data, new_cap);
  } else {
    new (end()) uint32_t(value);
  }
  ++size_;
}

void vector::pop_back() {
  data_[--size_].~uint32_t();
}

bool vector::empty() const {
  return size_ == 0;
}

size_t vector::capacity() const {
  return capacity_;
}

void vector::reserve(size_t new_capacity) {
  if (capacity_ < new_capacity) {
    uint32_t* new_data = nullptr;
    copy_data_array(data_, new_data, size_, new_capacity);
    assign_data_(new_data, new_capacity);
  }
}

void vector::shrink_to_fit() {
  if (size_ != capacity_) {
    uint32_t* new_data = nullptr;
    copy_data_array(data_, new_data, size_, size_);
    assign_data_(new_data, size_);
  }
}

void vector::clear() {
  clear_array(data_, size_);
  size_ = 0;
}

void vector::swap(vector& other) {
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(data_, other.data_);
}

vector::iterator vector::begin() {
  return data_;
}

vector::iterator vector::end() {
  return data_ + size_;
}

vector::const_iterator vector::begin() const {
  return data_;
}

vector::const_iterator vector::end() const {
  return data_ + size_;
}

vector::iterator vector::insert(const_iterator pos, uint32_t const& value) {
  size_t p = pos - begin();
  push_back(value);
  for (size_t i = size_ - 1; i > p; i--) {
    std::swap(data_[i - 1], data_[i]);
  }
  return begin() + p;
}

vector::iterator vector::erase(const_iterator pos) {
  return erase(pos, pos + 1);
}

vector::iterator vector::erase(const_iterator first, const_iterator last) {
  ptrdiff_t erase_size = last - first;
  if (erase_size != 0) {
    for (ptrdiff_t i = first - begin(); i < size_ - erase_size; i++) {
      std::swap(data_[i], data_[i + erase_size]);
    }
    for (ptrdiff_t i = 0; i < erase_size; i++) {
      pop_back();
    }
  }
  return (first - begin()) + begin();
}

void vector::resize(size_t new_size) {
  while (size() < new_size)
    push_back(0);
}
bool operator==(const vector& a, const vector& b) {
  if (a.size() == b.size()) {
    for (int i = 0; i < a.size(); i++) {
      if (a[i] != b[i]) {
        return false;
      }
    }
    return true;
  }
  return false;
}

void vector::reverse() {
  for (size_t i = 0; i < size_ / 2; i++) {
    auto x = data_[i];
    data_[i] = data_[size_ - i - 1];
    data_[size_ - i - 1] = x;
  }
}
