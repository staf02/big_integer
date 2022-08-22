#include "big_integer.h"
#include <cstddef>
#include <cstring>
#include <algorithm>
#include <ostream>
#include <stdexcept>

big_integer::big_integer() : sign(false), number() {}

//signed constructors

big_integer::big_integer(big_integer const& other) = default;

big_integer::big_integer(short a) : big_integer((long long) a) {}

big_integer::big_integer(int a) : big_integer((long long) a) {}

big_integer::big_integer(long a) : big_integer((long long) a) {}

big_integer::big_integer(long long a) : sign (a < 0) {
  if (a == 0) {
    number = vector();
    return;
  }
  bool to_add = false;
  if (a == INT64_MIN) {
    a += 1;
    a = -a;
    to_add = true;
  }
  if (a < 0)
    a = -a;
  (*this).push_back(a & UINT32_MAX);
  (*this).push_back(a >> 32);
  if (to_add) {
    (*this)--;
  }
  delete_zeros();
}

//unsigned constructors
big_integer::big_integer(unsigned short a) : big_integer((unsigned long long) a) {}

big_integer::big_integer(uint32_t a) : big_integer((unsigned long long) a) {}

big_integer::big_integer(unsigned long a) : big_integer((unsigned long long) a) {}

big_integer::big_integer(unsigned long long a) {
  sign = false;
  if (a == 0) {
    number = vector();
    return;
  }
  (*this).push_back((a & UINT32_MAX));
  (*this).push_back((a >> 32));
  delete_zeros();
}

//string constructor
big_integer::big_integer(std::string const& str) {
  number = vector(), sign = false;
  if (str == "" || str == "-") {
    throw std::invalid_argument("invalid argument");
  }
  if (str == "0" || str == "-0" || str.empty()) {
    return;
  }
  uint32_t tmp = 0;
  size_t start = (str[0] == '+' || str[0] == '-' ? 1 : 0);
  uint32_t multiplier = 1e9;
  for (size_t i = start; i < str.length(); i++) {
    if (str[i] < '0' || str[i] > '9') {
      sign = false, number.push_back(0);
      throw std::invalid_argument("invalid argument");
    }
    if (i - start == 9) {
      mul_short(multiplier);
      add_short(tmp);
      tmp = 0;
      start = i;
    }
    tmp *= 10;
    tmp += str[i] - '0';
  }
  multiplier = 1;
  for (size_t i = start; i < str.length(); i++) {
    multiplier *= 10;
  }
  mul_short(multiplier);
  add_short(tmp);
  if (str[0] == '-') {
    sign = true;
  }
  delete_zeros();
}

big_integer& big_integer::operator=(big_integer const& other) = default;

//destructor
big_integer::~big_integer() = default;

//sum & subtract

//short functions

void big_integer::add_short(uint32_t const& rhs) {
  if (length() == 0) {
    number.push_back(rhs);
    return;
  }
  uint32_t carry = 0;
  for (size_t i = 0; i < length(); i++) {
    uint64_t z = static_cast<int64_t>(number[i]) + (i == 0 ? rhs : 0) + carry;
    carry = (z > UINT32_MAX) ? 1 : 0;
    number[i] = static_cast<uint32_t>(z & UINT32_MAX);
  }
  if (carry > 0) {
    number.push_back(carry);
  }
}

void big_integer::mul_short(uint32_t const& rhs) {
  if (*this == 0 || rhs == 0) {
    *this = 0;
    return;
  }
  uint64_t carry = 0;
  for (size_t i = 0; i < length(); i++) {
    uint64_t tmp = static_cast<uint64_t>(number[i]) * rhs + carry;
    number[i] = tmp & UINT32_MAX;
    carry = tmp >> 32;
  }
  if (carry > 0)
    number.push_back(carry);
}

void big_integer::sub_short(uint32_t const& rhs) {
  if ((*this) <= 0) {
    add_short(rhs);
    return;
  }
  uint32_t carry = 0;
  for (size_t i = 0; i < length(); i++) {
    auto tmp = static_cast<int64_t>(number[i]) - carry - (i == 0 ? rhs : 0);
    number[i] = tmp < 0 ? UINT32_MAX + tmp + 1 : tmp;
    carry = tmp < 0 ? 1 : 0;
  }
  delete_zeros();
}
//some helpful functions

bool absolute_less(const big_integer& a, const big_integer& b) {
  if (a.length() != b.length()) {
    return a.length() < b.length();
  }
  for (size_t i = a.length(); i != 0; i--) {
    if (a[i - 1] != b[i - 1]) {
      return a[i - 1] < b[i - 1];
    }
  }
  return false;
}

void big_integer::absolute_increase(big_integer const& rhs) {
  uint64_t carry = 0;
  resize(rhs.length());
  for (size_t i = 0; i < length(); i++) {
    uint64_t z = static_cast<int64_t>(number[i]) + (i < rhs.length() ? rhs[i] : 0) + carry;
    carry = (z > UINT32_MAX) ? 1 : 0;
    number[i] = static_cast<uint32_t>(z & UINT32_MAX);
  }
  if (carry > 0) {
    number.push_back(carry);
  }
}

void big_integer::absolute_decrease(big_integer const& rhs, bool t) {
  if (!t) {
    uint32_t carry = 0;
    for (size_t i = 0; i < length(); i++) {
      auto tmp = static_cast<int64_t>(number[i]) - carry - (i < rhs.length() ? rhs[i] : 0);
      number[i] = tmp < 0 ? UINT32_MAX + tmp + 1 : tmp;
      carry = tmp < 0 ? 1 : 0;
    }
    delete_zeros();
  }
  else {
    uint32_t carry = 0;
    for (size_t i = 0; i < rhs.length(); i++) {
      auto tmp = static_cast<int64_t>(rhs[i]) - carry - (i < length() ? number[i] : 0);
      number[i] = tmp < 0 ? UINT32_MAX + tmp + 1 : tmp;
      carry = tmp < 0 ? 1 : 0;
    }
    sign = true;
    delete_zeros();
  }
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
  if (sign == rhs.sign) {
    absolute_increase(rhs);
    return (*this);
  }
  if (!absolute_less(*this, rhs)) {
    absolute_decrease(rhs, false);
  }
  else {
    absolute_decrease(rhs, true);
    if (!rhs.sign)
      sign ^= true;
  }
  return (*this);
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
  if (sign == rhs.sign) {
    bool t = absolute_less((*this), rhs);
    absolute_decrease(rhs, t);
    if (rhs.sign && t)
      sign ^= true;
    return (*this);
  }
  absolute_increase(rhs);
  return *this;
}

big_integer operator+(big_integer a, big_integer const& b) {
  return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
  return a -= b;
}

//multiply
big_integer& big_integer::operator*=(big_integer const& rhs) {
  if (*this == 0 || rhs == 0) {
    *this = 0;
    return *this;
  }
  big_integer res;
  res.sign = sign ^ rhs.sign;
  res.resize(length() + rhs.length());
  for (size_t i = 0; i < length(); i++) {
    uint64_t carry = 0;
    for (size_t j = 0; j < rhs.length(); j++) {
      uint64_t tmp = static_cast<uint64_t>(number[i]) * rhs[j] + carry + res[i + j];
      res[i + j] = tmp & UINT32_MAX;
      carry = tmp >> 32;
    }
    res[i + rhs.length()] += carry;
  }
  res.delete_zeros();
  *this = res;
  return *this;
}

big_integer operator*(big_integer a, big_integer const& b) {
  return a *= b;
}

//divide

uint32_t big_integer::short_div(uint32_t b) {
  uint64_t carry = 0;
  for (size_t i = this->length(); i != 0; i--) {
    uint64_t tmp = (carry << 32) + static_cast<uint64_t>((*this)[i - 1]);
    (*this)[i - 1] = tmp / b;
    carry = tmp % b;
  }
  delete_zeros();
  return carry;
}

uint32_t big_integer::trial(big_integer const &b) {
  uint64_t dividend = (static_cast<uint64_t>(number.back()) << 32) |
                      (static_cast<uint64_t>(number[length() - 2]));
  uint64_t divider = b.number.back();
  return (dividend / divider) & UINT32_MAX;
}

bool big_integer::smaller(big_integer const &b, size_t m) {
  for (size_t i = 1; i <= number.size(); i++) {
    uint32_t x = number[length() - i];
    uint32_t y = (m - i < b.length() ? b[m - i] : 0);
    if (x != y) {
      return x < y;
    }
  }
  return false;
}

void big_integer::difference(big_integer const &b, size_t m) {
  int64_t borrow = 0;
  uint64_t start = length() - m;
  for (size_t i = 0; i < m; i++) {
    int64_t diff = static_cast<int64_t>(number[start + i]) - (i < b.length() ? b[i] : 0) - borrow;
    number[start + i] = (diff < 0 ? diff + 1 + UINT32_MAX : diff);
    borrow = diff < 0;
  }
}

std::pair<big_integer, big_integer> big_integer::long_div(const big_integer& b) {
  bool ans_sign = sign ^ b.sign;
  bool this_sign = sign;

  sign = b.sign;
  if ((!b.sign && b > *this) || (b.sign && b < *this)) {
    sign = this_sign;
    this->delete_zeros();
    return {0, *this};
  }

  if (b.length() == 1) {
    big_integer sh = short_div(b[0]);
    sign = ans_sign;
    sh.sign = this_sign;

    this->delete_zeros();
    sh.delete_zeros();

    return {*this, sh};
  }

  uint32_t normalize_shift = (static_cast<uint64_t>(UINT32_MAX) + 1) / (static_cast<uint64_t>(b.number.back()) + 1);
  *this *= normalize_shift;
  big_integer divisor = b * normalize_shift;
  push_back(0);
  size_t m = divisor.length() + 1;
  big_integer ans, dq;
  ans.number.resize(length() - divisor.length());
  uint32_t qt = 0;

  for (size_t j = ans.length(); j != 0; j--) {
    qt = trial(divisor);
    dq = divisor * qt;

    while (smaller(dq, m)) {
      qt--;
      dq -= divisor;
    }
    ans[j - 1] = qt;
    difference(dq, m);
    pop_back();
  }

  ans.sign = ans_sign;
  ans.delete_zeros();

  sign = this_sign;
  this->short_div(normalize_shift);
  this->delete_zeros();

  return {ans, *this};
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
  return *this = long_div(rhs).first;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
  return *this = long_div(rhs).second;
}

big_integer operator/(big_integer a, big_integer const& b) {
  return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
  return a %= b;
}

//binary operations

void big_integer::additional_code() {
  for (uint32_t& cur : number) {
    cur = UINT32_MAX - cur;
  }
  sign = false;
  (*this)++;
}

void big_integer::binary_operation(big_integer b, binary_operations operation) {
  resize(b.length());
  b.resize(length());
  bool tmp_sign = false;
  switch (operation) {
  case AND: tmp_sign = b.sign & sign; break;
  case OR: tmp_sign = b.sign | sign; break;
  case XOR: tmp_sign = b.sign ^ sign; break;
  }
  if (sign)
    additional_code();
  if (b.sign)
    b.additional_code();
  for (size_t i = 0; i < length(); i++) {
    switch (operation) {
    case AND: number[i] &= b[i]; break;
    case OR: number[i] |= b[i]; break;
    case XOR: number[i] ^= b[i]; break;
    }
  }
  if (tmp_sign) {
    additional_code();
  }
  this->sign = tmp_sign;
  delete_zeros();
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
  binary_operation(rhs, AND);
  return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
  binary_operation(rhs, OR);
  return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
  binary_operation(rhs, XOR);
  return *this;
}

big_integer operator&(big_integer a, big_integer const& b) {
  return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
  return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
  return a ^= b;
}

//shifts

big_integer& big_integer::operator<<=(int rhs) {
  return *this = *this << rhs;
}

big_integer& big_integer::operator>>=(int rhs) {
  return *this = *this >> rhs;
}

big_integer operator<<(big_integer a, int b) {
  if (b < 0) {
    return a >> (-b);
  }
  a *= (static_cast<uint32_t>(1) << (b % 32));
  size_t tmp = b / 32;
  a.number.reverse();
  while (tmp) {
    a.push_back(0);
    tmp--;
  }
  a.number.reverse();
  a.delete_zeros();
  return a;
}

big_integer operator>>(big_integer a, int b) {
  if (b < 0) {
    return a << (-b);
  }
  a /= (static_cast<uint32_t>(1) << (b % 32));
  size_t tmp = b / 32;
  a.number.erase(a.number.begin(), a.number.begin() + std::min(tmp, a.length()));
  if (a.number.empty()) {
    a.push_back(0);
    a.sign = false;
  }
  a.delete_zeros();
  return a.sign ? a - 1 : a;
}


big_integer big_integer::operator+() const {
  return *this;
}

big_integer big_integer::operator-() const {
  if (length() == 1 && number[0] == 0) {
    return *this;
  }
  big_integer tmp = big_integer(*this);
  tmp.sign = !tmp.sign;
  tmp.delete_zeros();
  return tmp;
}

big_integer big_integer::operator~() const {
  return -(*this) - 1;
}

big_integer& big_integer::operator++() {
  add_short(1);
  return *this;
}

big_integer big_integer::operator++(int) {
  big_integer tmp = *this;
  add_short(1);
  return tmp;
}

big_integer& big_integer::operator--() {
  sub_short(1);
  return *this;
}

big_integer big_integer::operator--(int) {
  big_integer tmp = *this;
  sub_short(1);
  return tmp;
}

//comparators
bool operator==(big_integer const& a, big_integer const& b) {
  return a.sign == b.sign && a.number == b.number;
}

bool operator!=(big_integer const& a, big_integer const& b) {
  return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
  if (a.sign != b.sign) {
    return a.sign;
  }
  bool x = absolute_less(a, b);
  return a.sign ^ x;
}

bool operator>(big_integer const& a, big_integer const& b) {
  return !(a <= b);
}

bool operator<=(big_integer const& a, big_integer const& b) {
  return !(b < a);
}

bool operator>=(big_integer const& a, big_integer const& b) {
  return !(a < b);
}

//to string & output
std::string to_string(big_integer const& a) {
  if (a.length() == 0) {
    return "0";
  }
  std::string s = "";
  big_integer tmp(a);

  while (tmp != 0) {
    uint32_t z = tmp.short_div(1e9);
    std::string z_to_str = std::to_string(z);
    std::reverse(z_to_str.begin(), z_to_str.end());
    while (z_to_str.length() < 9)
      z_to_str += "0";
    s += z_to_str;
  }
  while (s.length() > 1 && s.back() == '0')
    s.pop_back();
  if (a.sign) {
    s.push_back('-');
  }
  std::reverse(s.begin(), s.end());
  return s;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
  return s << to_string(a);
}

//vector operations
void big_integer::delete_zeros() {
  while (number.size() > 0 && number.back() == 0)
    number.pop_back();
  if (number.size() == 0) {
    number = vector();
    sign = false;
  }
}

void big_integer::push_back(uint32_t value) {
  number.push_back(value);
}

void big_integer::pop_back() {
  number.pop_back();
}

void big_integer::resize(size_t new_size) {
  number.resize(new_size);
}

const size_t big_integer::length() const {
  return number.size();
}
