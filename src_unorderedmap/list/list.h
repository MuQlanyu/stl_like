#include <iostream>
#include <memory>
#include <vector>
/// List ///

template<class Key, class Value, class Hash, class Equal, class Alloc>
class UnorderedMap;

template<class T, class Alloc = std::allocator<T>>
class List {
 private:
  template<bool is_const = false>
  class base_iterator;

 public:
  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List() = default;
  List(size_t num) { create(num); }
  List(size_t num, const T& value) { create_value(num, value); }

  List(Alloc& alloc) : node_alloc_(alloc) {}
  List(size_t num, Alloc& alloc) : node_alloc_(alloc) { create(num); }
  List(size_t num, const T& value, Alloc& alloc) : node_alloc_(alloc) { create_value(num, value); }

  List(const List& copy);
  List& operator=(const List& assign);

  List(List&& copy) noexcept ;
  List& operator=(List&& assign) noexcept ;

  Alloc get_allocator() const { return node_alloc_; }

  size_t size() const { return size_; }

  void insert(iterator it, const T& value);
  void insert(iterator it, T&& value);
  void insert(iterator from, iterator to);
  void erase(iterator it);

  void push_back(const T& value);
  void push_back(T&& value);
  void pop_back();
  void push_front(const T& value);
  void push_front(T&& value);
  void pop_front();

  iterator end() { return iterator(&end_); }
  iterator begin() { return iterator(end_.next_); }
  const_iterator begin() const { return static_cast<const_iterator>(iterator(end_.next_)); }
  const_iterator end() const { return static_cast<const_iterator>(iterator(&end_)); }
  const_iterator cbegin() const { return static_cast<const_iterator>(begin()); }
  const_iterator cend() const { return static_cast<const_iterator>(end()); }

  const_reverse_iterator rbegin() const { return std::reverse_iterator(cend()); }
  reverse_iterator rbegin() { return std::reverse_iterator(end()); }
  const_reverse_iterator rend() const { return std::reverse_iterator(cbegin()); }
  reverse_iterator rend() { return std::reverse_iterator(begin()); }
  const_reverse_iterator crbegin() const { return std::reverse_iterator(cend()); }
  const_reverse_iterator crend() const { return std::reverse_iterator(cbegin()); }

  bool operator==(const List& other) const { return end_ == other.end_; }
  bool operator!=(const List& other) const { return end_ != other.end_; }

  ~List();
 private:
  struct BaseNode;
  struct Node;

  using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using node_alloc_traits = typename std::allocator_traits<NodeAlloc>;

  void create_value(size_t num, const T& value);
  void create(size_t num);
  void delete_nodes();

  BaseNode end_ = {&end_, &end_};
  size_t size_ = 0;
  [[no_unique_address]] NodeAlloc node_alloc_;

  template<class U, class AllocSwap>
  friend void swap(List<U, AllocSwap>& left, List<U, AllocSwap>& right);
};

/// Node ///

template<class T, class Alloc>
struct List<T, Alloc>::BaseNode {
  BaseNode* prev_ = nullptr;
  BaseNode* next_ = nullptr;

  BaseNode() = default;
  BaseNode(BaseNode* prev, BaseNode* next) : prev_(prev), next_(next) {}

  bool operator==(const List<T, Alloc>::BaseNode& other) const;
};

template<class T, class Alloc>
struct List<T, Alloc>::Node : public List<T, Alloc>::BaseNode {
  Node() = default;
  Node(const T& value) : value_(value) {};
  Node(T&& value) : value_(std::move(value)) {};
  Node(BaseNode* prev, BaseNode* next, T&& value) : List<T, Alloc>::BaseNode(prev, next),
                                                    value_(const_cast<T&&>(std::move(value))) {};
  Node(BaseNode* prev, BaseNode* next, const T& value) : List<T, Alloc>::BaseNode(prev, next), value_(value) {};

  T value_;
};

/// iterator ///

template<class T, class Alloc>
template<bool is_const>
class List<T, Alloc>::base_iterator {
 public:
  using value_type = std::conditional_t<is_const, const T, T>;
  using pointer = std::conditional_t<is_const, const T*, T*>;
  using difference_type = int;
  using reference = std::conditional_t<is_const, const T&, T&>;
  using iterator_category = std::bidirectional_iterator_tag;
  using Node_type = std::conditional_t<is_const, const Node, Node>;

  base_iterator() = delete;
  base_iterator(const BaseNode* node) : node_(const_cast<BaseNode*>(node)) {}

  base_iterator& operator++();
  base_iterator operator++(int);

  base_iterator& operator--();
  base_iterator operator--(int);

  reference operator*() const { return reinterpret_cast<Node_type*>(node_)->value_; }
  pointer operator->() const { return &reinterpret_cast<Node_type*>(node_)->value_; }

  operator base_iterator<true>() const { return const_iterator(node_); }

  bool operator==(const base_iterator& other) const { return node_ == other.node_; }
  bool operator!=(const base_iterator& other) const { return node_ != other.node_; }

  BaseNode* GetNode() const { return node_; }

 private:
  BaseNode* node_;
};
