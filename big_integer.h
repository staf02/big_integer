#pragma once

#include <iosfwd>
#include <string>
#include <vector>

struct big_integer {
  big_integer();
  big_integer(big_integer const& other);
  big_integer(int a);
  explicit big_integer(std::string const& str);
  big_integer(uint32_t a);
  big_integer(short a);
  big_integer(unsigned short a);
  big_integer(long long int a);
  big_integer(unsigned long long int a);
  big_integer(unsigned long a);
  big_integer(long a);
  ~big_integer();

  big_integer& operator=(big_integer const& other);

  big_integer& operator+=(big_integer const& rhs);
  big_integer& operator-=(big_integer const& rhs);
  big_integer& operator*=(big_integer const& rhs);
  big_integer& operator/=(big_integer const& rhs);
  big_integer& operator%=(big_integer const& rhs);

  big_integer& operator&=(big_integer const& rhs);
  big_integer& operator|=(big_integer const& rhs);
  big_integer& operator^=(big_integer const& rhs);

  big_integer& operator<<=(int rhs);
  big_integer& operator>>=(int rhs);

  big_integer operator+() const;
  big_integer operator-() const;
  big_integer operator~() const;

  big_integer& operator++();
  big_integer operator++(int);

  big_integer& operator--();
  big_integer operator--(int);

  friend big_integer operator+(big_integer a, big_integer const& b);
  friend big_integer operator-(big_integer a, big_integer const& b);
  friend big_integer operator/(big_integer a, big_integer const& b);
  friend big_integer operator*(big_integer a, big_integer const& b);
  friend big_integer operator%(big_integer a, big_integer const& b);

  friend big_integer operator&(big_integer a, big_integer const& b);
  friend big_integer operator|(big_integer a, big_integer const& b);
  friend big_integer operator^(big_integer a, big_integer const& b);

  friend big_integer operator<<(big_integer a, int b);
  friend big_integer operator>>(big_integer a, int b);

  friend bool operator==(big_integer const& a, big_integer const& b);
  friend bool operator!=(big_integer const& a, big_integer const& b);
  friend bool operator<(big_integer const& a, big_integer const& b);
  friend bool operator>(big_integer const& a, big_integer const& b);
  friend bool operator<=(big_integer const& a, big_integer const& b);
  friend bool operator>=(big_integer const& a, big_integer const& b);

  friend std::string to_string(big_integer const& a);
  void add_short(const uint32_t& rhs);
  void mul_short(const uint32_t& rhs);

private:
  std::vector<uint32_t> number;
  const size_t length() const;
  void push_back(uint32_t value);
  void pop_back();
  void delete_zeros();
  void absolute_increase(big_integer const&);
  void absolute_decrease(const big_integer& rhs, bool t);


  enum binary_operations {
    AND,
    OR,
    XOR
  };

  uint32_t const &operator[](size_t i) const {
    return number[i];
  }

  uint32_t &operator[](size_t i) {
    return number[i];
  }

  bool sign = false;
  void resize(size_t);
  void additional_code();
  uint32_t short_div(uint32_t b);
  std::pair<big_integer, big_integer> long_div(const big_integer& b);
  void difference(const big_integer& b, size_t m);
  uint32_t trial(big_integer const &b);
  bool smaller(const big_integer& b, size_t m);
  void binary_operation(big_integer b, binary_operations operation);
  friend bool absolute_less(const big_integer& a, const big_integer& b);
  void sub_short(const uint32_t& rhs);
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool absolute_less(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);


std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);
