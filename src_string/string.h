#pragma once

#include <iostream>
#include <cstring>
#include <algorithm>

class String {
 public:
  String(const char* str_initialisation);
  String(const int number, const char symbol);
  String() : data_(new char[1]{'\0'}) {}

  String(const String& str_copy);
  String& operator=(const String& str_copy);

  char& operator[](const size_t& i) { return data_[i]; }
  char operator[](const size_t& i) const { return data_[i]; }

  size_t length() const { return size_; }
  size_t size() const { return size_ + 1; }
  size_t capacity() const { return capacity_; }

  void push_back(const char symbol);
  void pop_back();

  char& front() { return data_[0]; }
  char& back() { return data_[size_ - 1]; }
  const char& front() const { return data_[0]; }
  const char& back() const { return data_[size_ - 1]; }

  String& operator+=(const String& add_str);
  String& operator+=(const char symbol);

  size_t find(const String& find_str) const;
  size_t rfind(const String& find_str) const;
  String substr(const int start, const int number) const;

  bool empty() const { return size_ == 0; }
  void clear();
  void shrink_to_fit();

  char*& data() { return data_; }
  char* data() const { return data_; }

  ~String() { delete[] data_; }

 private:
  char* data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 1;
};
