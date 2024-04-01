#pragma once

#include <iostream>
#include <iterator>
#include <type_traits>

///Deque declaration///
template<class T>
class Deque {
  template<bool is_const = false>
  class base_iterator;

 public:

  using const_iterator = base_iterator<true>;
  using iterator = base_iterator<false>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque();
  explicit Deque(size_t num);
  explicit Deque(size_t num, const T& object);

  Deque(const Deque& copy);
  Deque& operator=(const Deque& assignment);

  size_t size() const;

  const T& operator[](size_t index) const;
  T& operator[](size_t index);
  const T& at(size_t index) const;
  T& at(size_t index);

  void push_back(const T& object);
  void push_front(const T& object);
  void pop_back();
  void pop_front();

  const_iterator begin() const;
  iterator begin();
  const_iterator end() const;
  iterator end();
  const_iterator cbegin() const;
  const_iterator cend() const;

  const_reverse_iterator rbegin() const;
  reverse_iterator rbegin();
  const_reverse_iterator rend() const;
  reverse_iterator rend();
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  void insert(iterator iter, const T& input);
  void erase(iterator iter);

  ~Deque();
 private:
  struct Bucket;

  void Destruct(size_t size, base_iterator<false> iter, Bucket* arr) const;

  size_t bucket_size_;
  Bucket* array_ = nullptr;
  // Использую просто как три поля с определенными
  base_iterator<false> begin_;
  base_iterator<false> end_ = begin_;
};

///Bucket declaration///

template<class T>
struct Deque<T>::Bucket {
  static const size_t size_ = 8 * sizeof(int);
  T* arr_;
  Bucket();
  Bucket(const Bucket& other);
  ~Bucket() = default;
  operator T*();
};

///base_iterator declaration///

template<class T>
template<bool is_const>
class Deque<T>::base_iterator {
 public:
  using value_type = std::conditional_t<is_const, const T, T>;
  using pointer = std::conditional_t<is_const, const T*, T*>;
  using difference_type = int;
  using reference = std::conditional_t<is_const, const T&, T&>;
  using iterator_category = std::random_access_iterator_tag;
  using bucket_type = std::conditional_t<is_const, const Bucket, Bucket>;

  base_iterator(bucket_type* bucket, size_t index);
  base_iterator() = default;

  base_iterator& operator+=(int n);
  base_iterator operator+(int n) const;
  base_iterator& operator-=(int n);
  base_iterator operator-(int n) const;

  int operator-(const base_iterator& other) const;

  base_iterator& operator++();
  base_iterator operator++(int);

  base_iterator& operator--();
  base_iterator operator--(int);

  reference operator*() const;
  pointer operator->() const;

  pointer Pointer() const;
  bucket_type* BucketPointer() const;
  size_t InnerIndex() const;

  operator base_iterator<true>() const;

  bool operator<(const base_iterator& other) const;
  bool operator>(const base_iterator& other) const;
  bool operator<=(const base_iterator& other) const;
  bool operator>=(const base_iterator& other) const;
  bool operator==(const base_iterator& other) const;
  bool operator!=(const base_iterator& other) const;

 private:
  bucket_type* bucket_;
  pointer current_;
  size_t inner_index_;
};
