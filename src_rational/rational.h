#pragma once

class Rational {
 public:

  Rational(const BigInteger& numerator, const BigInteger& denominator = 1) :
      numerator_(numerator), denominator_(denominator),
      is_negative_(numerator.IsNegative() != denominator.IsNegative()) {};
  Rational(int numerator, int denominator = 1) :
      numerator_(numerator), denominator_(denominator),
      is_negative_((numerator < 0) != (denominator < 0)) {};
  Rational() = default;

  Rational operator-() const;

  Rational& operator+=(const Rational& other);
  Rational& operator-=(const Rational& other);
  Rational& operator*=(const Rational& other);
  Rational& operator/=(const Rational& other);

  std::string toString() const;
  std::string asDecimal(size_t precision = 0) const;

  explicit operator double();

 private:
  BigInteger numerator_ = 0;
  BigInteger denominator_ = 1;
  bool is_negative_ = false;

  friend bool operator<(const Rational& left, const Rational& right);
  friend bool operator==(const Rational& left, const Rational& right);

  void Gcd() { ::Gcd(numerator_, denominator_); }
  void Keep_positive();
};
