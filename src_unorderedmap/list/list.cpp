#include "list.h"

/// BaseNode ///

template<class T, class Alloc>
bool List<T, Alloc>::BaseNode::operator==(const List<T, Alloc>::BaseNode& other) const {
  return prev_ == other.prev_ && next_ == other.next_;
}

/// base_iterator ///

template<class T, class Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const>& List<T, Alloc>::base_iterator<is_const>::operator++() {
  node_ = node_->next_;
  return *this;
}

template<class T, class Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const> List<T, Alloc>::base_iterator<is_const>::operator++(int) {
  List<T, Alloc>::base_iterator<is_const> iter = *this;
  ++*this;
  return iter;
}

template<class T, class Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const>& List<T, Alloc>::base_iterator<is_const>::operator--() {
  node_ = node_->prev_;
  return *this;
}

template<class T, class Alloc>
template<bool is_const>
typename List<T, Alloc>::template base_iterator<is_const> List<T, Alloc>::base_iterator<is_const>::operator--(int) {
  List<T, Alloc>::base_iterator<is_const> iter = *this;
  --*this;
  return iter;
}

/// List ///

template<class T, class Alloc>
void List<T, Alloc>::create_value(size_t num, const T& value) {
  BaseNode* last_node = &end_;
  try {
    for (; size_ < num; ++size_) {
      last_node->next_ = node_alloc_.allocate(1);
      node_alloc_traits::construct(node_alloc_, reinterpret_cast<Node*>(last_node->next_), value);
      last_node->next_->prev_ = last_node;
      last_node = last_node->next_;
      last_node->next_ = &end_;
    }
    end_.prev_ = last_node;
  } catch (...) {
    for (size_t j = 0; j < size_; ++j) {
      node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(last_node->next_));
      node_alloc_.deallocate(reinterpret_cast<Node*>(last_node->next_), 1);
      last_node = last_node->prev_;
    }
    size_ = 0;
    throw;
  }
}

template<class T, class Alloc>
void List<T, Alloc>::create(size_t num) {
  BaseNode* last_node = &end_;
  try {
    for (; size_ < num; ++size_) {
      last_node->next_ = node_alloc_.allocate(1);
      node_alloc_traits::construct(node_alloc_, reinterpret_cast<Node*>(last_node->next_));
      last_node->next_->prev_ = last_node;
      last_node = last_node->next_;
      last_node->next_ = &end_;
    }
    end_.prev_ = last_node;
  } catch (...) {
    for (size_t j = 0; j < size_; ++j) {
      node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(last_node->next_));
      node_alloc_.deallocate(reinterpret_cast<Node*>(last_node->next_), 1);
      last_node = last_node->prev_;
    }
    size_ = 0;
    throw;
  }
}

template<class T, class Alloc>
void List<T, Alloc>::delete_nodes() {
  if (size_ == 0) return;
  BaseNode* ptr = end_.prev_;
  for (size_t i = 1; i < size_; ++i) {
    ptr = ptr->prev_;
    node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(ptr->next_));
    node_alloc_.deallocate(reinterpret_cast<Node*>(ptr->next_), 1);
  }
  node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(ptr));
  node_alloc_.deallocate(reinterpret_cast<Node*>(ptr), 1);
}

template<class T, class Alloc>
List<T, Alloc>::List(const List<T, Alloc>& copy) {
  NodeAlloc new_alloc = node_alloc_traits::select_on_container_copy_construction(copy.node_alloc_);
  Node* current_copy_node = reinterpret_cast<Node*>(copy.end_.next_);
  BaseNode* last_node = &end_;
  BaseNode* begin = end_.next_;
  size_t i = 0;
  try {
    for (; i < copy.size_; ++i) {
      last_node->next_ = new_alloc.allocate(1);
      node_alloc_traits::construct(new_alloc, reinterpret_cast<Node*>(last_node->next_), current_copy_node->value_);
      last_node->next_->prev_ = last_node;
      last_node = last_node->next_;
      current_copy_node = reinterpret_cast<Node*>(current_copy_node->next_);
    }
    last_node->next_ = &end_;
  } catch (...) {
    last_node = last_node->prev_;
    for (size_t j = 0; j < i; ++j) {
      node_alloc_traits::destroy(new_alloc, reinterpret_cast<Node*>(last_node->next_));
      new_alloc.deallocate(reinterpret_cast<Node*>(last_node->next_), 1);
      last_node = last_node->prev_;
    }
    end_.next_ = begin;
    throw;
  }

  delete_nodes();
  end_.prev_ = last_node;
  size_ = copy.size_;
  node_alloc_ = new_alloc;
}

template<class T, class Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& assign) {
  if (end_ == assign.end_) return *this;
  NodeAlloc new_alloc = node_alloc_;
  if (std::allocator_traits<Alloc>::propagate_on_container_copy_assignment::value) {
    new_alloc = assign.node_alloc_;
  }
  Node* current_copy_node = reinterpret_cast<Node*>(assign.end_.next_);
  BaseNode* last_node = &end_;
  BaseNode* begin = end_.next_;
  size_t i = 0;
  try {
    for (; i < assign.size_; ++i) {
      last_node->next_ = new_alloc.allocate(1);
      node_alloc_traits::construct(new_alloc, reinterpret_cast<Node*>(last_node->next_), current_copy_node->value_);
      last_node->next_->prev_ = last_node;
      last_node = last_node->next_;
      current_copy_node = reinterpret_cast<Node*>(current_copy_node->next_);
    }
    last_node->next_ = &end_;
  } catch (...) {
    last_node = last_node->prev_;
    for (size_t j = 0; j < i; ++j) {
      node_alloc_traits::destroy(new_alloc, reinterpret_cast<Node*>(last_node->next_));
      new_alloc.deallocate(reinterpret_cast<Node*>(last_node->next_), 1);
      last_node = last_node->prev_;
    }
    end_.next_ = begin;
    throw;
  }

  delete_nodes();
  end_.prev_ = last_node;
  size_ = assign.size_;
  node_alloc_ = new_alloc;
  return *this;
}

template<class T, class Alloc>
List<T, Alloc>::List(List&& copy) noexcept : size_(copy.size_), node_alloc_(std::move(copy.node_alloc_)) {
if (copy.size_ == 0) return;
std::swap(end_, copy.end_);
end_.next_->prev_ = &end_;
end_.prev_->next_ = &end_;
copy.end_ = {&copy.end_, &copy.end_};
copy.node_alloc_ = NodeAlloc();
copy.size_ = 0;
}

template<class T, class Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(List<T, Alloc>&& assign) noexcept {
if (*this == assign) return *this;
if (node_alloc_traits::propagate_on_container_move_assignment::value || assign.node_alloc_ == node_alloc_) {
std::swap(end_, assign.end_);
if (size_ == 0) assign.end_ = {&assign.end_, &assign.end_};
if (assign.size_ == 0) end_ = {&end_, &end_};
end_.next_->prev_ = &end_;
end_.prev_->next_ = &end_;
assign.end_.next_->prev_ = &assign.end_;
assign.end_.prev_->next_ = &assign.end_;
std::swap(size_, assign.size_);
auto tmp = std::move(node_alloc_);
node_alloc_ = std::move(assign.node_alloc_);
assign.node_alloc_ = std::move(tmp);
return *this;
}
Node* current_copy_node = reinterpret_cast<Node*>(assign.end_.next_);
BaseNode* last_node = &end_;
BaseNode* begin = end_.next_;
size_t i = 0;
try {
for (; i < assign.size_; ++i) {
last_node->next_ = node_alloc_.allocate(1);
node_alloc_traits::construct(node_alloc_,
reinterpret_cast<Node*>(last_node->next_),
std::move(current_copy_node->value_));
last_node->next_->prev_ = last_node;
last_node = last_node->next_;
current_copy_node = reinterpret_cast<Node*>(current_copy_node->next_);
}
last_node->next_ = &end_;
} catch (...) {
last_node = last_node->prev_;
for (size_t j = 0; j < i; ++j) {
node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(last_node->next_));
node_alloc_.deallocate(reinterpret_cast<Node*>(last_node->next_), 1);
last_node = last_node->prev_;
}
end_.next_ = begin;
throw;
}

delete_nodes();
end_.prev_ = last_node;
size_ = assign.size_;

}

template<class T, class Alloc>
void List<T, Alloc>::insert(List::iterator it, const T& value) {
  Node* ptr = node_alloc_.allocate(1);
  try {
    node_alloc_traits::construct(node_alloc_, ptr, it.GetNode()->prev_, it.GetNode(), value);
    it.GetNode()->prev_->next_ = reinterpret_cast<BaseNode*>(ptr);
    it.GetNode()->prev_ = reinterpret_cast<BaseNode*>(ptr);
    ++size_;
  } catch (...) {
    node_alloc_.deallocate(ptr, 1);
    throw;
  }
}

template<class T, class Alloc>
void List<T, Alloc>::insert(List::iterator it, T&& value) {
  Node* ptr = node_alloc_.allocate(1);
  try {
    node_alloc_traits::construct(node_alloc_, ptr, it.GetNode()->prev_, it.GetNode(), std::move(value));
    it.GetNode()->prev_->next_ = reinterpret_cast<BaseNode*>(ptr);
    it.GetNode()->prev_ = reinterpret_cast<BaseNode*>(ptr);
    ++size_;
  } catch (...) {
    node_alloc_.deallocate(ptr, 1);
    throw;
  }
}

template<class T, class Alloc>
void List<T, Alloc>::insert(List::iterator from, List::iterator to) {
  from.GetNode()->prev_->next_ = from.GetNode()->next_;
  from.GetNode()->next_->prev_ = from.GetNode()->prev_;
  from.GetNode()->prev_ = to.GetNode()->prev_;
  from.GetNode()->next_ = to.GetNode();
  to.GetNode()->prev_->next_ = from.GetNode();
  to.GetNode()->prev_ = from.GetNode();
}

template<class T, class Alloc>
void List<T, Alloc>::erase(List::iterator it) {
  if (it.GetNode() == &end_) return;
  it.GetNode()->prev_->next_ = it.GetNode()->next_;
  it.GetNode()->next_->prev_ = it.GetNode()->prev_;
  node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(it.GetNode()));
  node_alloc_.deallocate(reinterpret_cast<Node*>(it.GetNode()), 1);
  --size_;
}

template<class T, class Alloc>
void List<T, Alloc>::push_back(const T& value) {
  insert(end(), value);
}

template<class T, class Alloc>
void List<T, Alloc>::push_back(T&& value) {
  insert(end(), std::move(value));
}

template<class T, class Alloc>
void List<T, Alloc>::pop_back() {
  if (size_ > 0) erase(--end());
}

template<class T, class Alloc>
void List<T, Alloc>::push_front(const T& value) {
  insert(begin(), value);
}

template<class T, class Alloc>
void List<T, Alloc>::push_front(T&& value) {
  insert(begin(), std::move(value));
}

template<class T, class Alloc>
void List<T, Alloc>::pop_front() {
  if (size_ > 0) erase(begin());
}

template<class T, class Alloc>
List<T, Alloc>::~List() {
  delete_nodes();
}

template<class T, class Alloc>
void swap(List<T, Alloc>& left, List<T, Alloc>& right) {
  std::swap(left.size_, right.size_);
  std::swap(left.end_, right.end_);
  left.end_.next_->prev_ = &left.end_;
  left.end_.prev_->next_ = &left.end_;
  right.end_.next_->prev_ = &right.end_;
  right.end_.prev_->next_ = &right.end_;

  auto tmp = std::move(left.node_alloc_);
  left.node_alloc_ = std::move(right.node_alloc_);
  right.node_alloc_ = std::move(tmp);
}
