#include "deque.h"

/// Bucket ///

template<class T>
Deque<T>::Bucket::Bucket() : arr_(reinterpret_cast<T*>(new char[size_ * sizeof(T)])) {}

template<class T>
Deque<T>::Bucket::Bucket(const Deque::Bucket& other) {
  delete[] reinterpret_cast<char*>(arr_);
  arr_ = other.arr_;
}

template<class T>
Deque<T>::Bucket::operator T*() {
  return arr_;
}

/// base_iterator ///

template<class T>
template<bool is_const>
Deque<T>::base_iterator<is_const>::base_iterator(bucket_type* bucket, size_t index)
    : bucket_(bucket), current_((*bucket).arr_ + index), inner_index_(index) {}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator+=(int n) {
  if (n < 0) return *this -= -n;
  size_t num = n;
  inner_index_ += num;
  if (inner_index_ >= Bucket::size_) {
    bucket_ += inner_index_ / Bucket::size_;
    inner_index_ %= Bucket::size_;
    current_ = (*bucket_).arr_ + inner_index_;
  } else {
    current_ += num;
  }
  return *this;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator+(int n) const {
  Deque<T>::base_iterator<is_const> iter = *this;
  return iter += n;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator-=(int n) {
  if (n < 0) return *this += -n;
  size_t num = n;
  if (inner_index_ < num) {
    bucket_ -= (num - inner_index_ + Bucket::size_ - 1) / Bucket::size_;
    inner_index_ = (Bucket::size_ - ((num - inner_index_) % Bucket::size_)) % Bucket::size_;
    current_ = (*bucket_).arr_ + inner_index_;
  } else {
    inner_index_ -= num;
    current_ -= num;
  }
  return *this;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator-(int n) const {
  Deque<T>::base_iterator<is_const> iter = *this;
  return iter -= n;
}

template<class T>
template<bool is_const>
int Deque<T>::template base_iterator<is_const>::operator-(const base_iterator<is_const>& other) const {
  return (bucket_ - other.bucket_) * Bucket::size_ + (inner_index_ - other.inner_index_);
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator++() {
  if (inner_index_ + 1 == Deque<T>::Bucket::size_) {
    ++bucket_;
    current_ = (*bucket_).arr_;
    inner_index_ = 0;
  } else {
    ++inner_index_;
    ++current_;
  }
  return *this;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator++(int) {
  Deque<T>::base_iterator<is_const> iter = *this;
  ++*this;
  return iter;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>& Deque<T>::base_iterator<is_const>::operator--() {
  if (inner_index_ < 1) {
    --bucket_;
    current_ = *bucket_ + Deque<T>::Bucket::size_ - 1;
    inner_index_ = Deque<T>::Bucket::size_ - 1;
  } else {
    --current_;
    --inner_index_;
  }
  return *this;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const> Deque<T>::base_iterator<is_const>::operator--(int) {
  Deque<T>::base_iterator<is_const> iter = *this;
  --*this;
  return iter;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>::reference Deque<T>::base_iterator<is_const>::operator*() const {
  return *current_;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>::pointer Deque<T>::base_iterator<is_const>::operator->() const {
  return current_;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>::pointer Deque<T>::base_iterator<is_const>::Pointer() const {
  return current_;
}

template<class T>
template<bool is_const>
typename Deque<T>::template base_iterator<is_const>::bucket_type* Deque<T>::base_iterator<is_const>::BucketPointer() const {
  return bucket_;
}

template<class T>
template<bool is_const>
size_t Deque<T>::base_iterator<is_const>::InnerIndex() const {
  return inner_index_;
}

template<class T>
template<bool is_const>
Deque<T>::base_iterator<is_const>::operator base_iterator<true>() const {
  Deque<T>::base_iterator<true> iter(bucket_, inner_index_);
  return iter;
}

/// Begin/End ///

template<class T>
typename Deque<T>::const_iterator Deque<T>::begin() const {
  return static_cast<const_iterator>(begin_);
}

template<class T>
typename Deque<T>::iterator Deque<T>::begin() {
  return begin_;
}

template<class T>
typename Deque<T>::const_iterator Deque<T>::end() const {
  return static_cast<const_iterator>(end_);
}

template<class T>
typename Deque<T>::iterator Deque<T>::end() {
  return end_;
}

template<class T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
  return static_cast<const_iterator>(begin_);
}

template<class T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
  return static_cast<const_iterator>(end_);
}

template<class T>
typename Deque<T>::const_reverse_iterator Deque<T>::rbegin() const {
  return std::reverse_iterator(static_cast<const_iterator>(end_));
}

template<class T>
typename Deque<T>::reverse_iterator Deque<T>::rbegin() {
  return std::reverse_iterator(end_);
}

template<class T>
typename Deque<T>::const_reverse_iterator Deque<T>::rend() const {
  return std::reverse_iterator(static_cast<const_iterator>(begin_));
}

template<class T>
typename Deque<T>::reverse_iterator Deque<T>::rend() {
  return std::reverse_iterator(begin_);
}

template<class T>
typename Deque<T>::const_reverse_iterator Deque<T>::crbegin() const {
  return std::reverse_iterator(static_cast<const_iterator>(end_));
}

template<class T>
typename Deque<T>::const_reverse_iterator Deque<T>::crend() const {
  return std::reverse_iterator(static_cast<const_iterator>(begin_));
}

/// Deque ///

template<class T>
void Deque<T>::Destruct(size_t size, base_iterator<false> iter, Bucket* arr) const {
  for (size_t i = 0; i < size; ++i) {
    --iter;
    iter->~T();
  }
  delete[] arr;
}

template<class T>
Deque<T>::Deque() : bucket_size_(2), array_(new Bucket[2]), begin_(array_ + 1, 0), end_(begin_) {}

template<class T>
Deque<T>::Deque(size_t num) : bucket_size_(((num + Bucket::size_ - 1) / Bucket::size_)),
                              array_(new Bucket[bucket_size_]), begin_(array_ + bucket_size_ / 2, 0), end_(begin_ + num) {
  size_t i = 0;
  auto it = begin_;
  try {
    for (; i < num; ++i) {
      new ((it++).Pointer()) T();
    }
  } catch(...) {
    Destruct(i, it, array_);
    throw;
  }
}

template<class T>
Deque<T>::Deque(size_t num, const T& object) : bucket_size_(((num + Bucket::size_ - 1) / Bucket::size_)),
                                               array_(new Bucket[bucket_size_]), begin_(array_, 0), end_(begin_ + num) {
  size_t size = 0;
  iterator iter = begin_;
  try {
    for (; size < num; ++size) {
      new(iter.Pointer()) T(object);
      iter++;
    }
  } catch (...) {
    Destruct(size, iter, array_);
    throw;
  }
}

template<class T>
size_t Deque<T>::size() const {
  return end_ - begin_;
}

template<class T>
Deque<T>::Deque(const Deque<T>& copy) {
  size_t size = 0;
  Bucket* new_arr = new Bucket[copy.bucket_size_];
  iterator copy_iter = copy.begin_;
  iterator iter(new_arr + (copy.begin_.BucketPointer() - copy.array_), copy.begin_.InnerIndex());
  try {
    for (; size < copy.size(); ++size) {
      new((iter++).Pointer()) T(*(copy_iter++));
    }
  } catch (...) {
    Destruct(size, iter, new_arr);
    throw;
  }
  std::swap(array_, new_arr);
  if (new_arr != nullptr) {
    for (size_t i = 0; i < this->size(); ++i) {
      begin_->~T();
      ++begin_;
    }
    delete[] new_arr;
  }
  bucket_size_ = copy.bucket_size_;
  begin_ = iterator(array_ + (copy.begin_.BucketPointer() - copy.array_), copy.begin_.InnerIndex());
  end_ = begin_ + copy.size();
}

template<class T>
Deque<T>& Deque<T>::operator=(const Deque<T>& assignment) {
  if (array_ == assignment.array_) return *this;
  size_t size = 0;
  Bucket* new_arr = new Bucket[assignment.bucket_size_];
  iterator copy_iter = assignment.begin_;
  iterator iter(new_arr + (assignment.begin_.BucketPointer() - assignment.array_), assignment.begin_.InnerIndex());
  try {
    for (; size < assignment.size(); ++size) {
      new((iter++).Pointer()) T(*(copy_iter++));
    }
  } catch (...) {
    Destruct(size, iter, new_arr);
    throw;
  }
  std::swap(array_, new_arr);
  if (new_arr != nullptr) {
    for (size_t i = 0; i < this->size(); ++i) {
      begin_->~T();
      ++begin_;
    }
    delete[] new_arr;
  }
  bucket_size_ = assignment.bucket_size_;
  begin_ = iterator(array_ + (assignment.begin_.BucketPointer() - assignment.array_), assignment.begin_.InnerIndex());
  end_ = begin_ + assignment.size();
  return *this;
}

template<class T>
const T& Deque<T>::operator[](size_t index) const {
  return *(begin_ + index);
}

template<class T>
T& Deque<T>::operator[](size_t index) {
  return *(begin_ + index);
}

template<class T>
const T& Deque<T>::at(size_t index) const {
  if (index >= size()) throw std::out_of_range("");
  return *(begin_ + index);
}

template<class T>
T& Deque<T>::at(size_t index) {
  if (index >= size()) throw std::out_of_range("");
  return *(begin_ + index);
}

template<class T>
void Deque<T>::push_back(const T& object) {
  if (iterator(array_ + bucket_size_ - 1, Bucket::size_ - 1) == end_) {
    Bucket* new_arr = new Bucket[3 * bucket_size_];
    for (size_t i = 0; i < bucket_size_; ++i) {
      new_arr[i + bucket_size_] = array_[i];
    }
    try {
      new(*(new_arr + 2 * bucket_size_)) T(object);
    } catch (...) {
      delete[] new_arr;
    }
    begin_ = iterator(new_arr + bucket_size_ + (begin_.BucketPointer() - array_), begin_.InnerIndex());
    end_ = iterator(new_arr + bucket_size_ + (end_.BucketPointer() - array_) + 1, 0);
    std::swap(array_, new_arr);
    delete[] new_arr;
    bucket_size_ = 3 * bucket_size_;
  } else {
    try {
      new((end_++).Pointer()) T(object);
    } catch(...) {
      --end_;
      throw;
    }
  }
}

template<class T>
void Deque<T>::push_front(const T& object) {
  if (iterator(array_, 0) == begin_) {
    Bucket* new_arr = new Bucket[3 * bucket_size_];
    for (size_t i = 0; i < bucket_size_; ++i) {
      new_arr[i + bucket_size_] = array_[i];
    }
    try {
      new (*(new_arr + bucket_size_ - 1) + Bucket::size_ - 1) T(object);
    } catch (...) {
      delete[] new_arr;
      throw;
    }
    begin_ = iterator(new_arr + bucket_size_ - 1, Bucket::size_ - 1);
    end_ = iterator(new_arr + bucket_size_ + (end_.BucketPointer() - array_), end_.InnerIndex());
    std::swap(array_, new_arr);
    delete[] new_arr;
    bucket_size_ = 3 * bucket_size_;
  } else {
    try {
      new((--begin_).Pointer()) T(object);
    } catch(...) {
      ++begin_;
      throw;
    }
  }
}

template<class T>
void Deque<T>::pop_back() {
  end_->~T();
  --end_;
}

template<class T>
void Deque<T>::pop_front() {
  begin_->~T();
  begin_++;
}

template<class T>
void Deque<T>::insert(Deque::iterator iter, const T& input) {
  int shift = iter - begin_;
  push_back(input); // iter может стать инвалидироваться
  iter = begin_ + shift;
  for (auto it = end_ - 1; it > iter; --it) {
    *it = *(it - 1);
  }
  *iter = input;
}

template<class T>
void Deque<T>::erase(Deque::iterator iter) {
  for (++iter; iter < end_; ++iter) {
    *(iter - 1) = *iter;
  }
  pop_back();
}

template<class T>
Deque<T>::~Deque() {
  Destruct(size(), end_, array_);
}

/// Отдельные функции ///

template<class T>
template<bool is_const>
bool Deque<T>::template base_iterator<is_const>::operator<(const typename Deque<T>::template base_iterator<is_const>& other) const {
  if (bucket_ != other.bucket_) {
    return bucket_ < other.bucket_;
  }
  return inner_index_ < other.inner_index_;
}

template<class T>
template<bool is_const>
bool Deque<T>::template base_iterator<is_const>::operator>(const typename Deque<T>::template base_iterator<is_const>& other) const {
  return other < *this;
}

template<class T>
template<bool is_const>
bool Deque<T>::template base_iterator<is_const>::operator<=(const typename Deque<T>::template base_iterator<is_const>& other) const {
  return !(*this > other);
}

template<class T>
template<bool is_const>
bool Deque<T>::template base_iterator<is_const>::operator>=(const typename Deque<T>::template base_iterator<is_const>& other) const {
  return !(*this < other);
}

template<class T>
template<bool is_const>
bool Deque<T>::template base_iterator<is_const>::operator==(const typename Deque<T>::template base_iterator<is_const>& other) const {
  if (bucket_ != other.bucket_) {
    return false;
  }
  return inner_index_ == other.inner_index_;
}

template<class T>
template<bool is_const>
bool Deque<T>::template base_iterator<is_const>::operator!=(const typename Deque<T>::template base_iterator<is_const>& other) const {
  return !(*this == other);
}
