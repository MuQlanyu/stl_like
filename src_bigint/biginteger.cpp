#include "biginteger.h"

BigInteger::BigInteger(int new_big_integer) {
  is_negative_ = (new_big_integer < 0);
  digits_.push_back(abs(new_big_integer));
  if (digits_[0] > kMaxInt_) {
    digits_.push_back(digits_[0] / kMaxInt_);
    digits_[0] %= kMaxInt_;
  }
}

BigInteger::BigInteger(const BigInteger& copy) : digits_(copy.digits_),
                                                 is_negative_(copy.is_negative_) {}

BigInteger& BigInteger::operator=(const BigInteger& assignment) {
  digits_ = assignment.digits_;
  is_negative_ = assignment.is_negative_;
  return *this;
}

void BigInteger::StringToBigInteger(const std::string& str) {
  is_negative_ = false;
  digits_.clear();
  if (str.length() == 0) {
    return;
  }
  long long first_symbol = 0;
  if (str[0] == '-') {
    is_negative_ = true;
    ++first_symbol;
  }
  while (first_symbol < static_cast<long long>(str.length()) && str[first_symbol] == '0') {
    ++first_symbol;
  }
  for (long long i = str.length(); i > 0; i -= kMaxLen_) {
    if (i - first_symbol >= static_cast<long long>(kMaxLen_)) {
      digits_.push_back(std::stoi(str.substr(i - kMaxLen_, kMaxLen_)));
    } else {
      if (i > first_symbol) {
        digits_.push_back(std::stoi(str.substr(first_symbol, i - first_symbol)));
      }
    }
  }
}

bool BigInteger::IsNull() const {
  if (digits_.size() != 1) {
    return false;
  }
  return digits_[0] == 0;
}

void BigInteger::Pow(size_t power) {
  QuickE9(power / 9);
  ShiftLeft(power % 9);
}

void BigInteger::QuickE9(size_t power) {
  size_t tmp = digits_.size();
  digits_.resize(digits_.size() + power);
  for (size_t i = tmp; i > 0; --i) {
    digits_[i + power - 1] = digits_[i - 1];
    digits_[i - 1] = 0;
  }
}

void BigInteger::ShiftLeft(size_t pow) {
  int shift = 1;
  size_t initial_size = digits_.size();
  for (size_t i = 0; i < pow; ++i) {
    shift *= 10;
  }
  shift = kMaxInt_ / shift;
  if (digits_.back() >= shift) {
    digits_.push_back(0);
  }
  for (size_t i = initial_size; i > 0; --i) {
    if (i < digits_.size()) {
      digits_[i] += digits_[i - 1] / shift;
    }
    digits_[i - 1] = (digits_[i - 1] % shift) * (kMaxInt_ / shift);
  }
}

void BigInteger::DeleteZeros() {
  while (digits_.size() > 1 && digits_.back() == 0) {
    digits_.pop_back();
  }
}

BigInteger BigInteger::operator-() const {
  BigInteger sign_change = *this;
  sign_change.is_negative_ = !sign_change.is_negative_;
  return sign_change;
}

int BigInteger::DigitsCmp(const BigInteger& other) const {
  if (digits_.size() < other.digits_.size()) {
    return -1;
  } else if (digits_.size() > other.digits_.size()) {
    return 1;
  }
  for (size_t i = digits_.size(); i > 0; --i) {
    if (digits_[i - 1] < other.digits_[i - 1]) {
      return -1;
    } else if (digits_[i - 1] > other.digits_[i - 1]) {
      return 1;
    }
  }
  return 0;
}

bool operator<(const BigInteger& left, const BigInteger& right) {
  if (left.is_negative_ != right.is_negative_) {
    return left.is_negative_;
  }
  if (left.digits_.size() != right.digits_.size()) {
    return left.is_negative_ != (left.digits_.size() < right.digits_.size());
  }
  for (size_t i = left.digits_.size(); i > 0; --i) {
    if (left.digits_[i - 1] != right.digits_[i - 1]) {
      return left.is_negative_ != (left.digits_[i - 1] < right.digits_[i - 1]);
    }
  }
  return false;
}

bool operator>(const BigInteger& left, const BigInteger& right) {
  return right < left;
}

bool operator<=(const BigInteger& left, const BigInteger& right) {
  return !(left > right);
}

bool operator>=(const BigInteger& left, const BigInteger& right) {
  return !(left < right);
}

bool operator==(const BigInteger& left, const BigInteger& right) {
  if (left.IsNull() && right.IsNull()) {
    return true;
  }
  if (left.is_negative_ != right.is_negative_) {
    return false;
  }
  return left.digits_ == right.digits_;
}

bool operator!=(const BigInteger& left, const BigInteger& right) {
  return !(left == right);
}

void BigInteger::Addition(const BigInteger& other) {
  if (digits_.size() < other.digits_.size()) {
    digits_.resize(other.digits_.size());
  }
  bool carry = false;
  for (size_t i = 0; i < other.digits_.size(); ++i) {
    if (carry) {
      ++digits_[i];
      carry = false;
    }
    digits_[i] += other.digits_[i];
    if (digits_[i] > kMaxInt_) {
      carry = true;
      digits_[i] -= kMaxInt_;
    }
  }
  if (carry) {
    digits_.push_back(1);
  }
}

void BigInteger::Subtraction(const BigInteger& other) {
  size_t carry;
  if (DigitsCmp(other) != -1) {
    for (size_t i = other.digits_.size(); i > 0; --i) {
      if (digits_[i - 1] < other.digits_[i - 1]) {
        carry = i;
        while (digits_[carry] == 0) {
          digits_[carry++] += kMaxInt_ - 1;
        }
        --digits_[carry];
        digits_[i - 1] -= other.digits_[i - 1] - kMaxInt_;
      } else {
        digits_[i - 1] -= other.digits_[i - 1];
      }
    }
  } else {
    is_negative_ = !is_negative_;
    digits_.resize(other.digits_.size());
    for (size_t i = other.digits_.size(); i > 0; --i) {
      if (digits_[i - 1] > other.digits_[i - 1]) {
        carry = i;
        while (other.digits_[carry] == 0) {
          digits_[carry++] += kMaxInt_ - 1;
        }
        --digits_[carry];
        digits_[i - 1] = other.digits_[i - 1] + kMaxInt_ - digits_[i - 1];
      } else {
        digits_[i - 1] = other.digits_[i - 1] - digits_[i - 1];
      }
    }
  }
  DeleteZeros();
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
  if (is_negative_ == other.is_negative_) {
    Addition(other);
  } else {
    Subtraction(other);
  }
  return *this;
}

BigInteger operator+(BigInteger first, const BigInteger& second) {
  return first += second;
}
//Пришлось скопировать код, чтобы не создавать копию
BigInteger& BigInteger::operator-=(const BigInteger& other) {
  if (is_negative_ == other.is_negative_) {
    Subtraction(other);
  } else {
    Addition(other);
  }
  return *this;
}

BigInteger operator-(BigInteger first, const BigInteger& second) {
  return first -= second;
}

BigInteger& BigInteger::operator*=(const BigInteger& other) {
  is_negative_ = (is_negative_ != other.is_negative_);
  size_t init_size = digits_.size();
  digits_.resize(digits_.size() + other.digits_.size());
  long long ll_this;
  long long multiplication;
  for (size_t ind = init_size; ind > 0; --ind) {
    ll_this = static_cast<long long>(digits_[ind - 1]);
    multiplication = other.digits_[0] * ll_this;
    digits_[ind - 1] = static_cast<int>(multiplication % kMaxInt_);
    digits_[ind] += static_cast<int>(multiplication / kMaxInt_);
    for (size_t delta_ind = 1; delta_ind < other.digits_.size(); ++delta_ind) {
      multiplication = static_cast<long long>(other.digits_[delta_ind]) * ll_this;
      if (digits_[ind - 1 + delta_ind] > kMaxInt_) {
        digits_[ind + delta_ind] += digits_[ind - 1 + delta_ind] / kMaxInt_;
        digits_[ind - 1 + delta_ind] %= kMaxInt_;
      }
      digits_[ind - 1 + delta_ind] += static_cast<int>(multiplication % kMaxInt_);
      if (digits_[ind - 1 + delta_ind] > kMaxInt_) {
        digits_[ind + delta_ind] += digits_[ind - 1 + delta_ind] / kMaxInt_;
        digits_[ind - 1 + delta_ind] %= kMaxInt_;
      }
      digits_[ind + delta_ind] += static_cast<int>(multiplication / kMaxInt_);
    }
  }
  DeleteZeros();
  return *this;
}

BigInteger operator*(BigInteger first, const BigInteger& second) {
  return first *= second;
}

BigInteger& BigInteger::operator/=(const BigInteger& other) {
  if (other.IsNull()) {
    return *this;
  }
  //Проверка, деления меньшего на большее
  if (digits_.size() < other.digits_.size()) {
    return *this = 0;
  }
  if (digits_.size() == other.digits_.size()) {
    for (size_t i = digits_.size(); i > 0; --i) {
      if (digits_[i - 1] != other.digits_[i - 1]) {
        if (digits_[i - 1] < other.digits_[i - 1]) {
          return *this = 0;
        } else {
          break;
        }
      }
    }
  }

  is_negative_ = (is_negative_ != other.is_negative_);
  BigInteger copy = *this;
  BigInteger intermediate_result = 0;
  BigInteger cur_result = 0;
  BigInteger other_copy = other;
  if (other_copy.IsNegative()) {
    other_copy = -other_copy;
  }
  digits_.clear();
  int l;
  int r;
  int m;
  int delta = copy.digits_.size() - other_copy.digits_.size();
  intermediate_result.digits_.clear();
  for (size_t i = delta; i < copy.digits_.size(); ++i) {
    intermediate_result.digits_.push_back(copy.digits_[i]);
  }

  if (intermediate_result < other_copy) {
    --delta;
    intermediate_result.QuickE9(1);
    intermediate_result.digits_[0] = copy.digits_[delta];
  }
  for (size_t i = delta + 1; i > 0; --i) {
    l = 0;
    r = kMaxInt_;
    if (intermediate_result >= other_copy) {
      l = 1;
      while (r - l > 1) {
        m = (r + l) >> 1;
        cur_result = m * other_copy;
        if (cur_result <= intermediate_result) {
          l = m;
        } else {
          r = m;
        }
      }
      intermediate_result -= l * other_copy;
    }
    intermediate_result.QuickE9(1);
    intermediate_result.digits_[0] = copy.digits_[i - 2];
    intermediate_result.DeleteZeros();
    QuickE9(1);
    digits_[0] = l;
  }
  return *this;
}

BigInteger operator/(BigInteger first, const BigInteger& second) {
  return first /= second;
}

BigInteger& BigInteger::operator%=(const BigInteger& other) {
  return *this -= *this / other * other;
}

BigInteger operator%(BigInteger first, const BigInteger& second) {
  return first %= second;
}

BigInteger& BigInteger::operator++() {
  return *this += 1;
}
BigInteger BigInteger::operator++(int) {
  BigInteger copy = *this;
  *this += 1;
  return copy;
}
BigInteger& BigInteger::operator--() {
  return *this -= 1;
}
BigInteger BigInteger::operator--(int) {
  BigInteger copy = *this;
  *this -= 1;
  return copy;
}

std::string BigInteger::toString() const {
  std::string str;
  if (IsNull()) {
    return "0";
  }
  size_t reservation = std::to_string(digits_.back()).length() +
      9 * (digits_.size() - 1) + (is_negative_ ? 1 : 0);
  str.reserve(reservation);
  if (is_negative_) {
    str += '-';
  }
  str += std::to_string(digits_.back());
  for (size_t i = digits_.size() - 1; i > 0; --i) {
    str += std::string(kMaxLen_ - std::to_string(digits_[i - 1]).length(), '0')
        + std::to_string(digits_[i - 1]);
  }
  return str;
}

BigInteger::operator bool() const {
  return !IsNull();
}

BigInteger::operator int() const {
  if (is_negative_) {
    return -digits_[0];
  }
  return digits_[0];
}

BigInteger operator "" _bi(unsigned long long conv) {
  BigInteger result = conv;
  return result;
}

std::ostream& operator<<(std::ostream& ostr, const BigInteger& big_integer) {
  return ostr << big_integer.toString();
}
std::istream& operator>>(std::istream& istr, BigInteger& big_integer) {
  std::string read_istream;
  istr >> read_istream;
  big_integer.StringToBigInteger(read_istream);
  return istr;
}

void Gcd(BigInteger& left, BigInteger& right) {
  BigInteger left_copy = left, right_copy = right;
  while (right_copy && left_copy) {
    if (left_copy < right_copy) {
      right_copy %= left_copy;
    } else {
      left_copy %= right_copy;
    }
    if (left_copy == 1 or right_copy == 1) {
      return;
    }
  }
  if (!right_copy) {
    left /= left_copy;
    right /= left_copy;
  } else {
    left /= right_copy;
    right /= right_copy;
  }
}
