#pragma once

#include <iostream>
#include <vector>

class BigInteger {
 public:
  BigInteger(int new_big_integer);
  BigInteger(const char* new_big_integer) { StringToBigInteger(new_big_integer); }
  BigInteger(const std::string& new_big_integer) { StringToBigInteger(new_big_integer); }
  BigInteger() = default;

  BigInteger(const BigInteger& copy);
  BigInteger& operator=(const BigInteger& assignment);

  bool IsNegative() const { return is_negative_; }
  void Pow(size_t power);
  size_t Size() const { return digits_.size(); }

  BigInteger operator-() const;

  BigInteger& operator+=(const BigInteger& other);
  BigInteger& operator-=(const BigInteger& other);
  BigInteger& operator*=(const BigInteger& other);
  BigInteger& operator/=(const BigInteger& other);
  BigInteger& operator%=(const BigInteger& other);

  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);

  std::string toString() const;
  explicit operator bool() const;
  explicit operator int() const;

 private:

  static const size_t kMaxLen_ = 9;
  static const int kMaxInt_ = 1e9;

  std::vector<int> digits_;
  bool is_negative_ = false;

  friend bool operator<(const BigInteger& left, const BigInteger& right);
  friend bool operator==(const BigInteger& left, const BigInteger& right);
  friend std::istream& operator>>(std::istream& istr, BigInteger& big_integer);

  void StringToBigInteger(const std::string& str);
  bool IsNull() const;
  void DeleteZeros();
  void ChangeSign() { is_negative_ = !is_negative_; }
  void KeepPositiveNull() { if (IsNull()) is_negative_ = false; }
  void QuickE9(size_t power);
  void ShiftLeft(size_t shift);

  void Addition(const BigInteger& other); //Одинаковый знак
  void Subtraction(const BigInteger& other); //Разный знак
  int DigitsCmp(const BigInteger& other) const;
};
