#include "rational.h"

void Rational::Keep_positive() {
  if (numerator_.IsNegative()) {
    numerator_ = -numerator_;
  }
  if (denominator_.IsNegative()) {
    denominator_ = -denominator_;
  }
}

bool operator<(const Rational& left, const Rational& right) {
  if (left.numerator_ == 0 && right.numerator_ == 0) {
    return false;
  }
  if (left.is_negative_ != right.is_negative_) {
    return left.is_negative_;
  }
  return left.numerator_ * right.denominator_ < left.denominator_ * right.numerator_;
};
bool operator>(const Rational& left, const Rational& right) {
  return right < left;
};
bool operator<=(const Rational& left, const Rational& right) {
  return !(left > right);
};
bool operator>=(const Rational& left, const Rational& right) {
  return !(left < right);
};
bool operator==(const Rational& left, const Rational& right) {
  if (!left.numerator_ && !right.numerator_) {
    return true;
  }
  if (left.is_negative_ != right.is_negative_) {
    return false;
  }
  return left.numerator_ == right.numerator_ && left.denominator_ == right.denominator_;
};
bool operator!=(const Rational& left, const Rational& right) {
  return !(left == right);
};

Rational Rational::operator-() const {
  Rational copy = *this;
  copy.is_negative_ = !is_negative_;
  return copy;
}

Rational& Rational::operator+=(const Rational& other) {
  if (is_negative_ == other.is_negative_) {
    numerator_ = numerator_ * other.denominator_ + other.numerator_ * denominator_;
    denominator_ *= other.denominator_;
    Keep_positive();
    Gcd();
    return *this;
  }
  return *this -= -other;
}

Rational operator+(Rational first, const Rational& second) {
  return first += second;
}

Rational& Rational::operator-=(const Rational& other) {
  if (is_negative_ == other.is_negative_) {
    is_negative_ = (*this < other);
    numerator_ = numerator_ * other.denominator_ - other.numerator_ * denominator_;
    denominator_ *= other.denominator_;
    Keep_positive();
    Gcd();
    return *this;
  }
  return *this += -other;
}

Rational operator-(Rational first, const Rational& second) {
  return first -= second;
}

Rational& Rational::operator*=(const Rational& other) {
  is_negative_ = (is_negative_ != other.is_negative_);
  numerator_ *= other.numerator_;
  denominator_ *= other.denominator_;
  Keep_positive();
  Gcd();
  return *this;
}

Rational operator*(Rational first, const Rational& second) {
  return first *= second;
}

Rational& Rational::operator/=(const Rational& other) {
  if (other == *this) {
    return *this = 1;
  }
  if (!numerator_) {
    return *this;
  }
  is_negative_ = (is_negative_ != other.is_negative_);
  numerator_ *= other.denominator_;
  denominator_ *= other.numerator_;
  Keep_positive();
  Gcd();
  return *this;
}

Rational operator/(Rational first, const Rational& second) {
  return first /= second;
}

std::string Rational::toString() const {
  std::string str_rational;
  if (is_negative_ && !numerator_.IsNegative()) {
    str_rational += '-';
  }
  str_rational += numerator_.toString();
  if (denominator_ != 1) {
    str_rational += '/' + denominator_.toString();
  }
  return str_rational;
}

std::string Rational::asDecimal(size_t precision) const {
  BigInteger result = numerator_;
  size_t delta = denominator_.Size() * 9;
  result.Pow(precision + delta);
  result /= denominator_;
  std::string str_result;
  if (numerator_ > denominator_) {
    if (is_negative_) {
      str_result += '-';
    }
    str_result += result.toString();
    str_result = str_result.substr(0, str_result.length() - precision - delta) + '.' +
        str_result.substr(str_result.length() - precision - delta, precision);
  } else {
    str_result = result.toString();
    if (is_negative_) {
      str_result = "-0." + std::string(precision + delta - str_result.length(), '0')
          + str_result.substr(0, str_result.length() - delta);
    } else {
      str_result = "0." + std::string(precision + delta - str_result.length(), '0')
          + str_result.substr(0, str_result.length() - delta);
    }
  }
  return str_result;
}

Rational::operator double() {
  return std::stod(asDecimal(10));
}
