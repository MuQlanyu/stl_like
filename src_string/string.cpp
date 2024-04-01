#include "string.h"

String::String(const char* str_initialisation) :
    data_(new char[strlen(str_initialisation) + 1]),
    size_(strlen(str_initialisation)), capacity_(size_ + 1) {
  std::copy(str_initialisation, str_initialisation + size_, data_);
  data_[size_] = '\0';
}

String::String(const int number, const char symbol) : data_(new char[number + 1]),
                                                      size_(number),
                                                      capacity_(number + 1) {
  std::fill(data_, data_ + number, symbol);
  data_[number] = '\0';
}

String::String(const String& str_copy) {
  delete[] data_;
  data_ = new char[str_copy.capacity_];
  size_ = str_copy.size_;
  capacity_ = str_copy.capacity_;
  std::copy(str_copy.data(), str_copy.data() + size_, data_);
  data_[size_] = '\0';
}

bool operator==(const String& first_str, const String& second_str) {
  if (first_str.length() != second_str.length()) return false;
  return std::strcmp(first_str.data(), second_str.data()) == 0;
}

bool operator!=(const String& first_str, const String& second_str) {
  return !(first_str == second_str);
}

bool operator<(const String& first_str, const String& second_str) {
  return std::strcmp(first_str.data(), second_str.data()) < 0;
}

bool operator>(const String& first_str, const String& second_str) {
  return (second_str < first_str);
}

bool operator<=(const String& first_str, const String& second_str) {
  return !(first_str > second_str);
}

bool operator>=(const String& first_str, const String& second_str) {
  return !(first_str < second_str);
}

String& String::operator=(const String& str_copy) {
  if (str_copy == *this) return *this;
  delete[] data_;
  size_ = str_copy.length();
  capacity_ = str_copy.capacity();
  data_ = new char[capacity_];
  std::copy(str_copy.data(), str_copy.data() + size_, data_);
  data_[size_] = '\0';
  return *this;
}

void String::push_back(const char symbol) {
  if (size_ + 1 < capacity_) {
    data_[size_] = symbol;
    data_[++size_] = '\0';
    return;
  }
  capacity_ *= 2;
  char* new_str = new char[capacity_];
  std::copy(data_, data_ + size_, new_str);
  new_str[size_] = symbol;
  new_str[++size_] = '\0';
  std::swap(data_, new_str);
  delete[] new_str;
}

void String::pop_back() {
  if (size_ > 0) {
    data_[--size_] = '\0';
  }
}

String& String::operator+=(const String& add_str) {
  size_ += add_str.size_;
  if (size_ + 1 <= capacity_) {
    std::copy(add_str.data_, add_str.data_ + add_str.size_, data_ + size_ - add_str.size_);
    data_[size_] = '\0';
    return *this;
  }
  capacity_ = (size_ + 1) * 2;
  char* new_str = new char[capacity_];
  std::copy(data_, data_ + size_ - add_str.size_, new_str);
  std::copy(add_str.data_, add_str.data_ + add_str.size_, new_str + size_ - add_str.size_);
  new_str[size_] = '\0';
  std::swap(data_, new_str);
  delete[] new_str;
  return *this;
}

String& String::operator+=(const char symbol) {
  push_back(symbol);
  return *this;
}

String operator+(String first, const String& second) {
  return first += second;
}

String operator+(const char symbol, const String& str) {
  return String(1, symbol) += str;
}

String operator+(String str, const char symbol) {
  return str += String(1, symbol);
}

size_t String::find(const String& find_str) const {
  size_t prefix_len = 0;
  size_t ind = 0;
  while (ind + prefix_len < size_ && find_str[prefix_len] != '\0') {
    if (data_[ind + prefix_len] == find_str[prefix_len]) {
      ++prefix_len;
    } else {
      ++ind;
      prefix_len = 0;
    };
  }
  if (find_str[prefix_len] == '\0') {
    return ind;
  }
  return size_;
}

size_t String::rfind(const String& find_str) const {
  size_t suffix_len = 0;
  size_t ind = 0;
  size_t find_str_len = find_str.length();
  while (suffix_len + ind < size_ && suffix_len != find_str_len) {
    if (data_[size_ - ind - suffix_len - 1] ==
        find_str[find_str_len - suffix_len - 1]) {
      ++suffix_len;
    } else {
      suffix_len = 0;
      ++ind;
    }
  }
  if (suffix_len == find_str_len) {
    return size_ - ind - suffix_len;
  }
  return size_;
}

String String::substr(const int start, const int number) const {
  String sub_str = String(number, ' ');
  std::copy(data_ + start, data_ + start + number, sub_str.data_);
  return sub_str;
}

void String::clear() {
  size_ = 0;
  if (data_ != nullptr) data_[0] = '\0';
}

void String::shrink_to_fit() {
  capacity_ = size_ + 1;
  char* new_str = new char[capacity_];
  std::copy(data_, data_ + capacity_, new_str);
  std::swap(data_, new_str);
  delete[] new_str;
}

std::ostream& operator<<(std::ostream& ostr, const String& str_out) {
  for (size_t i = 0; i < str_out.length(); ++i) {
    ostr << str_out[i];
  }
  return ostr;
}

std::istream& operator>>(std::istream& istr, String& str_in) {
  char c;
  str_in.clear();
  while (istr.peek() == ' ' || istr.peek() == '\n' || istr.peek() == '\t') istr.get(c);
  while (istr.peek() != ' ' && istr.peek() != '\n' && istr.peek() != '\t' && istr.peek() != EOF) {
    istr >> c;
    str_in.push_back(c);
  }
  return istr;
}
