#include <iostream>
#include <memory>

template<class T>
class WeakPtr;

struct BaseControlBlock {
  BaseControlBlock(size_t rcnt, size_t wcnt) : rcount(rcnt), wcount(wcnt) {}

  virtual const void* ptr() const = 0;
  virtual void delete_object() = 0;
  virtual void deallocate() = 0;

  virtual ~BaseControlBlock() = default;

  size_t rcount;
  size_t wcount;
};

template<typename T, typename U>
struct check {
  static const bool value = std::is_base_of_v<T, U> || std::is_same_v<T, U>;
};

/// SharedPtr ///
template<class T>
class SharedPtr {
 public:
  SharedPtr() = default;

  template<class U, std::enable_if_t<check<T, U>::value, int> = 0>
  SharedPtr(U* ptr) : cb_(new RegularControlBlock<U>{1, 0, ptr}) {}

  template<class U, class Deleter, std::enable_if_t<check<T, U>::value, int> = 0>
  SharedPtr(U* ptr, Deleter del) : cb_(new RegularControlBlock<U, Deleter>{1, 0, ptr, del}) {}

  template<class U, class Deleter, class Alloc, std::enable_if_t<check<T, U>::value, int> = 0>
  SharedPtr(U* ptr, Deleter del, Alloc alloc);

  SharedPtr(const SharedPtr& copy) : cb_(copy.cb_) { if (cb_ != nullptr) ++(cb_->rcount); }
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  SharedPtr(const SharedPtr<U>& copy) : cb_(copy.GetControlBlock()) { if (cb_ != nullptr) ++(cb_->rcount); }

  SharedPtr(SharedPtr&& copy)  noexcept : cb_(copy.cb_) { copy.cb_ = nullptr; }
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  explicit SharedPtr(SharedPtr<U>&& copy) : cb_(copy.GetControlBlock()) { copy.SetControlBlock(nullptr); }

  SharedPtr& operator=(const SharedPtr<T>& assign);
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  SharedPtr& operator=(const SharedPtr<U>& assign);

  SharedPtr& operator=(SharedPtr<T>&& assign) noexcept ;
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  SharedPtr& operator=(SharedPtr<U>&& assign);

  T& operator*() const { return *get(); }
  T* operator->() const { return get(); }
  T* get() const { return (cb_ == nullptr ? nullptr : reinterpret_cast<T*>(const_cast<void*>(cb_->ptr()))); }

  size_t use_count() const { return (cb_ != nullptr ? cb_->rcount : 0); }

  void reset();
  template<class U>
  void reset(U* new_ptr);

  template<class U>
  void swap(SharedPtr<U>& other);

  BaseControlBlock* GetControlBlock() const { return cb_; }
  void SetControlBlock(BaseControlBlock* ptr) { cb_ = ptr; }

  ~SharedPtr();

 private:
  template<class U, class Deleter = std::default_delete<U>, class Alloc = std::allocator<U>>
  struct RegularControlBlock;

  template<class U, class Alloc = std::allocator<U>>
  struct MakeControlBlock;

  explicit SharedPtr(BaseControlBlock* cb) : cb_(cb) { if (cb_ != nullptr) ++(cb_->rcount); }
  template<class U, class Alloc, typename std::enable_if_t<check<T, U>::value, int> = 0>
  explicit SharedPtr(MakeControlBlock<U, Alloc>* cb) : cb_(cb) {}

  template<class U, class... Args>
  friend SharedPtr<U> makeShared(Args&& ... args);

  template<class U, class Alloc, class... Args>
  friend SharedPtr<U> allocateShared(Alloc& alloc, Args&& ... args);

  template<class U, class W>
  friend bool operator==(const SharedPtr<U>& left, const SharedPtr<W>& right);

  friend WeakPtr<T>;

  BaseControlBlock* cb_ = nullptr;
};

/// ControlBlock ///

template<class T>
template<class U, class Deleter, class Alloc>
struct SharedPtr<T>::RegularControlBlock : BaseControlBlock {
  RegularControlBlock(size_t rcnt, size_t wcnt, U* ptr) : BaseControlBlock(rcnt, wcnt), object(ptr) {}
  RegularControlBlock(size_t rcnt, size_t wcnt, U* ptr, Deleter deleter) :
      BaseControlBlock(rcnt, wcnt), object(ptr), deleter(deleter) {}
  RegularControlBlock(size_t rcnt, size_t wcnt, U* ptr, Deleter deleter, Alloc alloc) :
      BaseControlBlock(rcnt, wcnt), object(ptr), deleter(deleter), alloc(alloc) {}

  const void* ptr() const override { return reinterpret_cast<const void*>(object); }
  void delete_object() override { deleter(object); }
  void deallocate() override;

  U* object;
  Deleter deleter;
  Alloc alloc;
};

template<class T>
template<class U, class Alloc>
struct SharedPtr<T>::MakeControlBlock : BaseControlBlock {
  template<class... Args>
  MakeControlBlock(size_t rcnt, size_t wcnt, Args&& ... args);

  template<class... Args>
  MakeControlBlock(size_t rcnt, size_t wcnt, Alloc alloc, Args&& ... args);

  const void* ptr() const override { return reinterpret_cast<const void*>(object); }
  void delete_object() override { std::allocator_traits<Alloc>::destroy(alloc, reinterpret_cast<U*>(object)); }
  void deallocate() override;

  char object[sizeof(U)];
  Alloc alloc;
};

/// WeakPtr ///

template<class T>
class WeakPtr {
 public:
  WeakPtr() = default;

  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  WeakPtr(const SharedPtr<U>& ptr) : cb_(ptr.GetControlBlock()) { if (cb_ != nullptr) ++(cb_->wcount); };

  WeakPtr(const WeakPtr<T>& copy) : cb_(copy.cb_) { if (cb_ != nullptr) ++(cb_->wcount); };
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  WeakPtr(const WeakPtr<U>& copy) : cb_(copy.GetControlBlock()) { if (cb_ != nullptr) ++(cb_->wcount); };

  WeakPtr(WeakPtr<T>&& copy)  noexcept : cb_(copy.cb_) { copy.SetControlBlock(nullptr); };
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  explicit WeakPtr(WeakPtr<U>&& copy) : cb_(copy.GetControlBlock()) { copy.SetControlBlock(nullptr); };

  WeakPtr& operator=(const WeakPtr<T>& assign);
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  WeakPtr& operator=(const WeakPtr<U>& assign);

  WeakPtr& operator=(WeakPtr<T>&& assign) noexcept ;
  template<class U, typename std::enable_if_t<check<T, U>::value, int> = 0>
  WeakPtr& operator=(WeakPtr<U>&& assign);

  bool expired() const { return (cb_ == nullptr ? true : cb_->rcount == 0); }
  SharedPtr<T> lock() const { return SharedPtr<T>(cb_); }
  size_t use_count() const { return (cb_ == nullptr ? 0 : cb_->rcount); }

  BaseControlBlock* GetControlBlock() const { return cb_; }
  void SetControlBlock(BaseControlBlock* ptr) { cb_ = ptr; }

  ~WeakPtr();
 private:
  BaseControlBlock* cb_ = nullptr;
};
