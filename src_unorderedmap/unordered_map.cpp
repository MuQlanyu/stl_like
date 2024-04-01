#include "unordered_map.h"

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap<Key, Value, Hash, Equal, Alloc>& copy) {
  arr_.resize(default_arr_size, end());
  NodeAlloc new_alloc = alloc_traits::select_on_container_copy_construction(copy.alloc_);
  list_ = List<Node, NodeAlloc>(new_alloc);
  rehash(copy.bucket_size());
  try {
    for (const_iterator iter = copy.begin(); iter != copy.end(); ++iter) {
      if (arr_[iter.hash() % bucket_size()] == end()) {
        list_.push_back(iter.GetNode());
      } else {
        list_.insert(arr_[iter.hash() % bucket_size()].Iterator(), iter.GetNode());
      }
      --arr_[iter.hash() % bucket_size()];
    }
  } catch (...) {
    Node* ptr;
    for (auto it = list_.begin(); it != list_.end(); ++it) {
      if (arr_[it->hash % bucket_size()] != end()) arr_[it->hash % bucket_size()] = end();
      ptr = &(*it);
      alloc_traits::destroy(alloc_, ptr);
      alloc_.deallocate(ptr, 1);
    }
    throw;
  }
  alloc_ = std::move(new_alloc);
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(const UnorderedMap<Key, Value, Hash, Equal, Alloc>& assign) {
  if (list_ == assign.list_) return *this;
  NodeAlloc new_alloc = alloc_;
  if (alloc_traits::propagate_on_container_copy_assignment::value) {
    new_alloc = assign.alloc_;
  }
  List<Node, NodeAlloc> new_list(new_alloc);
  using it_alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<iterator>;
  arr_ = std::vector<iterator, it_alloc>(assign.bucket_size(), end());
  for (const_iterator iter = assign.begin(); iter != assign.end(); ++iter) {
    if (arr_[iter.hash() % assign.bucket_size()] == end()) {
      new_list.push_back(iter.GetNode());
      arr_[iter.hash() % assign.bucket_size()] = iterator(--new_list.end());
    } else {
      new_list.insert(arr_[iter.hash() % assign.bucket_size()].Iterator(), iter.GetNode());
      arr_[iter.hash() % assign.bucket_size()] = --arr_[iter.hash() % assign.bucket_size()];
    }
  }

  alloc_ = std::move(new_alloc);
  list_ = std::move(List<Node, NodeAlloc>());
  list_ = std::move(new_list);
  return *this;
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>&
UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(UnorderedMap<Key, Value, Hash, Equal, Alloc>&& assign) noexcept{
if (list_ == assign.list_) return *this;
using it_alloc = typename std::allocator_traits<Alloc>::template rebind_alloc<iterator>;
arr_ = std::vector<iterator, it_alloc>(assign.bucket_size(), end());
if (alloc_traits::propagate_on_container_move_assignment::value || assign.alloc_ == alloc_) {
UnorderedMap copy = std::move(assign);
auto tmp = std::move(copy.list_);
copy.list_ = std::move(list_);
list_ = std::move(tmp);
for (size_t i = 0; i < copy.arr_.size(); ++i) {
if (copy.arr_[i] != assign.end()) {
arr_[i] = std::move(copy.arr_[i]);
}
}
return *this;
} else {
List<Node, NodeAlloc> new_list(alloc_);
for (const_iterator iter = assign.begin(); iter != assign.end(); ++iter) {
if (arr_[iter.hash() % assign.bucket_size()] == end()) {
new_list.push_back(iter.GetNode());
arr_[iter.hash() % assign.bucket_size()] = iterator(--new_list.end());
} else {
new_list.insert(arr_[iter.hash() % assign.bucket_size()].Iterator(), iter.GetNode());
arr_[iter.hash() % assign.bucket_size()] = --arr_[iter.hash() % assign.bucket_size()];
}
}

list_ = std::move(List<Node, NodeAlloc>());
list_ = std::move(new_list);
return *this;
}
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
  iterator iter = find(key);
  if (iter == end()) throw std::out_of_range("key not found");
  return iter->second;
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(UnorderedMap::NodeType&& insert) {
  return emplace(const_cast<Key&&>(std::move(insert.first)), std::move(insert.second));
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class InputIt>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIt first, InputIt last) {
  for (; first != last; ++first) {
    emplace(std::forward<decltype(*first)>(*first));
  }
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args&& ... args) {
  if (static_cast<double>(size() + 1) / arr_.size() > max_factor) rehash(2 * bucket_size());
  char a[sizeof(NodeType)];
  NodeType* elem_ptr = reinterpret_cast<NodeType*>(&a);
  std::allocator_traits<ElemAlloc>::construct(elem_alloc_, elem_ptr, std::forward<Args>(args)...);
  Node fake = {std::move(*elem_ptr), 0};
  Node* ptr = &fake;
  ptr->hash = hash_(ptr->node.first);
  if (arr_[ptr->hash % bucket_size()] == end()) {
    size_t hash = ptr->hash;
    list_.insert(end().Iterator(), std::move(*ptr));
    arr_[hash % bucket_size()] = --end();
    return {--end(), true};
  }
  iterator iter = arr_[ptr->hash % bucket_size()];
  while (iter.hash() % bucket_size() == ptr->hash % bucket_size()) {
    if (equal_(iter->first, ptr->node.first)) return {iter, false};
    ++iter;
  }
  list_.insert(iter.Iterator(), std::move(*ptr));
  return {--iter, true};
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(UnorderedMap::iterator pos) {
  auto return_it = ++pos;
  if (pos == begin() || (--pos).hash() % bucket_size() == pos.hash() % bucket_size()) {
    if (return_it.hash() % bucket_size() != pos.hash() % bucket_size()) arr_[pos.hash() % bucket_size()] = end();
    else arr_[pos.hash() % bucket_size()] = return_it;
  }
  list_.erase(pos.Iterator());
  return return_it;
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(UnorderedMap::iterator first, UnorderedMap::iterator last) {
  iterator delete_iter = first;
  for (++first; first != last; delete_iter = first, ++first) {
    erase(delete_iter);
  }
  erase(delete_iter);
  return last;
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator
UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key) {
  size_t hash = hash_(key);
  UnorderedMap::iterator iter = arr_[hash % bucket_size()];
  if (iter == end()) return end();
  while (iter.hash() % bucket_size() == hash % bucket_size()) {
    if (equal_(iter->first, key)) return iter;
    ++iter;
  }
  return end();
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(size_t count) {
  size_t bucket_rehash = static_cast<size_t>(count / max_factor) + 1;
  if (bucket_rehash < bucket_size()) return;
  rehash(bucket_rehash);
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash(size_t bucket_count) {
  if (bucket_count == 0) bucket_count = static_cast<size_t>(default_arr_size);
  std::vector<iterator, typename std::allocator_traits<Alloc>::template rebind_alloc<iterator>>
      new_arr(bucket_count, end());
  iterator iter = begin();
  for (size_t i = 0; i < list_.size(); ++i) {
    ++iter;
    if (new_arr[(--iter).hash() % bucket_count] != end()) {
      list_.insert((--iter).Iterator(), ++(new_arr[(--iter).hash() % bucket_count].Iterator()));
    } else {
      new_arr[(--iter).hash() % bucket_count] = --iter;
    }
  }
  std::swap(arr_, new_arr);
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
void swap(UnorderedMap<Key, Value, Hash, Equal, Alloc>& left,
          UnorderedMap<Key, Value, Hash, Equal, Alloc>& right) {
  swap(left.list_, right.list_);
  std::swap(left.arr_, right.arr_);
}
