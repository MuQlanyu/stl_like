#pramga once

#include <iostream>
#include <memory>
#include <vector>

#include "list/list.h"

///////////////////////////////////////////////////////////////////////////////////

///////////////////////           UNORDERED MAP            ////////////////////////

///////////////////////////////////////////////////////////////////////////////////

template<class Key, class Value, class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>, class Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 private:
  template<bool is_const>
  class base_iterator;
 public:
  using NodeType = std::pair<const Key, Value>;
  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;

  UnorderedMap() { arr_.resize(default_arr_size, end()); };

  UnorderedMap(const UnorderedMap& copy);
  UnorderedMap(UnorderedMap&& copy) noexcept = default;

  UnorderedMap& operator=(const UnorderedMap& assign);
  UnorderedMap& operator=(UnorderedMap&& assign) noexcept;

  Value& operator[](const Key& key) { return emplace(key, Value()).first->second; }
  Value& operator[](Key&& key) { return emplace(std::move(key), Value()).first->second; }

  Value& at(const Key& key);
  const Value& at(const Key& key) const { return const_cast<UnorderedMap*>(this)->at(key); }

  [[nodiscard]] size_t size() const { return list_.size(); }

  std::pair<iterator, bool> insert(const NodeType& insert) { return emplace(insert); }
  std::pair<iterator, bool> insert(NodeType&& insert);

  template<class InputIt>
  void insert(InputIt first, InputIt last);

  template<class... Args>
  std::pair<iterator, bool> emplace(Args&& ... args);

  iterator erase(iterator pos);
  iterator erase(iterator first, iterator last);

  iterator find(const Key& key);
  const_iterator find(const Key& key) const { return const_cast<UnorderedMap*>(this)->find(key); }

  void reserve(size_t count);
  double load_factor() { return static_cast<double>(size()) / bucket_size(); }
  double max_load_factor() { return max_factor; }

  iterator end() { return iterator(list_.end()); }
  iterator begin() { return iterator(list_.begin()); }
  const_iterator begin() const { return const_iterator(list_.begin()); }
  const_iterator end() const { return const_iterator(list_.end()); }
  const_iterator cbegin() const { return const_iterator(list_.begin()); }
  const_iterator cend() const { return const_iterator(list_.end()); }

  ~UnorderedMap() = default;

 private:
  struct Node;
  using ElemAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<NodeType>;
  using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using alloc_traits = typename std::allocator_traits<NodeAlloc>;

  void rehash(size_t bucket_count);
  [[nodiscard]] size_t bucket_size() const { return arr_.size(); }
  constexpr static const double max_factor = 1;
  static const int default_arr_size = 1000;

  [[no_unique_address]] Hash hash_;
  [[no_unique_address]] Equal equal_;
  [[no_unique_address]] NodeAlloc alloc_;
  [[no_unique_address]] ElemAlloc elem_alloc_;
  std::vector<iterator, typename std::allocator_traits<Alloc>::template rebind_alloc<iterator>> arr_;
  List<Node, NodeAlloc> list_;

  friend void swap(UnorderedMap& left, UnorderedMap& right);
};

template<class Key, class Value, class Hash, class Equal, class Alloc>
struct UnorderedMap<Key, Value, Hash, Equal, Alloc>::Node {
  NodeType node;
  size_t hash;
  Node() = default;
  explicit Node(NodeType&& node) : node(const_cast<Key&&>(std::move(node.first)), std::move(node.second),
                                        std::move(node.second)) {}
  explicit Node(const NodeType& node) : node(node) {}
  Node(NodeType&& nod, size_t hash) : node(const_cast<Key&&>(std::move(nod.first)), std::move(nod.second)),
                                      hash(hash) {}
  Node(const NodeType& node, size_t hash) : node(node), hash(hash) {}
  Node(Node&& copy) noexcept : node(const_cast<Key&&>(std::move(copy.node.first)), std::move(copy.node.second)),
                               hash(std::move(copy.hash)) {} //надо бы занулить node
  Node(const Node& copy) : node(copy.node), hash(copy.hash) {}
};

/// iterator ///

template<class Key, class Value, class Hash, class Equal, class Alloc>
template<bool is_const>
class UnorderedMap<Key, Value, Hash, Equal, Alloc>::base_iterator {
 public:
  using value_type = std::conditional_t<is_const, const NodeType, NodeType>;
  using pointer = std::conditional_t<is_const, const NodeType*, NodeType*>;
  using difference_type = int;
  using reference = std::conditional_t<is_const, const NodeType&, NodeType&>;
  using iterator_category = std::bidirectional_iterator_tag;
  using node_type = std::conditional_t<is_const, const Node, Node>;
  using iter_type = std::conditional_t<is_const,
                                       typename List<Node, NodeAlloc>::const_iterator,
                                       typename List<Node, NodeAlloc>::iterator>;

  base_iterator() = delete;
  explicit base_iterator(const iter_type& construct) : iter(const_cast<iter_type&>(construct)) {}

  base_iterator& operator++() {
    ++iter;
    return *this;
  };
  base_iterator operator++(int) { return base_iterator(iter++); };

  base_iterator& operator--() {
    --iter;
    return *this;
  }
  base_iterator operator--(int) { return base_iterator(iter--); };

  reference operator*() const { return iter->node; }
  pointer operator->() const { return &(iter->node); }

  operator base_iterator<true>() const { return static_cast<const_iterator>(iter); }

  bool operator==(const base_iterator& other) const { return iter == other.iter; }
  bool operator!=(const base_iterator& other) const { return iter != other.iter; }

  size_t hash() { return iter->hash; }
  iter_type Iterator() { return iter; }
  node_type& GetNode() { return *iter; }

 private:
  iter_type iter;
};
