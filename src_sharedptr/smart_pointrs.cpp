/// RegularControlBlock ///

template<class T>
template<class U, class Deleter, class Alloc>
void SharedPtr<T>::RegularControlBlock<U, Deleter, Alloc>::deallocate() {
  using RegCB = RegularControlBlock<U, Deleter, Alloc>;
  using BlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<RegCB>;
  BlockAlloc block_alloc(alloc);
  block_alloc.deallocate(this, 1);
}

/// MakeControlBlock ///

template<class T>
template<class U, class Alloc>
template<class... Args>
SharedPtr<T>::MakeControlBlock<U, Alloc>::MakeControlBlock(size_t rcnt, size_t wcnt, Args&& ...args) :
    BaseControlBlock(rcnt, wcnt) {
  new(reinterpret_cast<U*>(object)) U(std::forward<Args>(args)...);
}

template<class T>
template<class U, class Alloc>
template<class... Args>
SharedPtr<T>::MakeControlBlock<U, Alloc>::MakeControlBlock(size_t rcnt, size_t wcnt, Alloc alloc, Args&& ... args) :
    BaseControlBlock(rcnt, wcnt), alloc(alloc) {
  new(reinterpret_cast<U*>(object)) U(std::forward<Args>(args)...);
}

template<class T>
template<class U, class Alloc>
void SharedPtr<T>::MakeControlBlock<U, Alloc>::deallocate() {
  using RegCB = MakeControlBlock<U, Alloc>;
  using BlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<RegCB>;
  BlockAlloc block_alloc(alloc);
  block_alloc.deallocate(this, 1);
}

/// SharedPtr ///

template<class T>
template<class U, class Deleter, class Alloc, typename std::enable_if_t<check<T, U>::value, int>>
SharedPtr<T>::SharedPtr(U* ptr, Deleter del, Alloc alloc) {
  using RegCB = RegularControlBlock<U, Deleter, Alloc>;
  using BlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<RegCB>;
  BlockAlloc block_alloc(alloc);
  cb_ = block_alloc.allocate(1);
  new(reinterpret_cast<RegularControlBlock<U, Deleter, Alloc>*>(cb_)) RegCB(1, 0, ptr, del, alloc);
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& assign) {
  if (assign == *this) return *this;
  if (cb_ != nullptr) this->~SharedPtr();
  cb_ = assign.cb_;
  if (cb_ != nullptr) ++(cb_->rcount);
  return *this;
}

template<class T>
template<class U, typename std::enable_if_t<check<T, U>::value, int>>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& assign) {
  if (assign == *this) return *this;
  if (cb_ != nullptr) this->~SharedPtr();
  cb_ = assign.GetControlBlock();
  if (cb_ != nullptr) ++(cb_->rcount);
  return *this;
}

template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& assign) noexcept {
if (cb_ != nullptr) this->~SharedPtr();
cb_ = assign.cb_;
assign.cb_ = nullptr;
return *this;
}

template<class T>
template<class U, typename std::enable_if_t<check<T, U>::value, int>>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& assign) {
  check<T, U>();
  if (cb_ != nullptr) this->~SharedPtr();
  cb_ = assign.GetControlBlock();
  assign.SetControlBlock(nullptr);
  return *this;
}

template<class T>
void SharedPtr<T>::reset() {
  if (cb_ != nullptr) this->~SharedPtr();
  cb_ = nullptr;
}

template<class T>
template<class U>
void SharedPtr<T>::reset(U* new_ptr) {
  if (cb_ != nullptr) this->~SharedPtr();
  cb_ = new RegularControlBlock<U>{1, 0, new_ptr};
}

template<class T>
SharedPtr<T>::~SharedPtr() {
  if (cb_ == nullptr) return;
  --(cb_->rcount);
  if (cb_->rcount == 0) {
    cb_->delete_object();
    if (cb_->wcount == 0) {
      cb_->deallocate();
      cb_ = nullptr;
    }
  }
}

template<class T>
template<class U>
void SharedPtr<T>::swap(SharedPtr<U>& other) {
  BaseControlBlock* tmp = std::move(other.GetControlBlock());
  other.SetControlBlock(cb_);
  cb_ = tmp;
}

template<class U, class W>
bool operator==(const SharedPtr<U>& left, const SharedPtr<W>& right) { return left.cb_ == right.cb_; }

template<class U, class W>
bool operator!=(const SharedPtr<U>& left, const SharedPtr<W>& right) { return !(left == right);  }

/// WeakPtr ///

template<class T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& assign) {
  if (cb_ != nullptr) this->~WeakPtr();

  cb_ = assign.cb_;
  if (cb_ != nullptr) ++(cb_->wcount);
  return *this;
}

template<class T>
template<class U, typename std::enable_if_t<check<T, U>::value, int>>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<U>& assign) {
  if (cb_ != nullptr) this->~WeakPtr();

  cb_ = assign.GetControlBlock();
  if (cb_ != nullptr) ++(cb_->wcount);
  return *this;
}

template<class T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<T>&& assign) noexcept {
if (cb_ != nullptr) this->~WeakPtr();

cb_ = assign.cb_;
assign.cb_ = nullptr;
return *this;
}

template<class T>
template<class U, typename std::enable_if_t<check<T, U>::value, int>>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<U>&& assign) {
  if (cb_ != nullptr) this->~WeakPtr();

  cb_ = assign.GetControlBlock();
  assign.SetControlBlock(nullptr);
  return *this;
}

template<class T>
WeakPtr<T>::~WeakPtr() {
  if (cb_ == nullptr) return;
  --(cb_->wcount);
  if (cb_->rcount == 0 && cb_->wcount == 0) {
    cb_->deallocate();
    cb_ = nullptr;
  }
}

/// Функции ///

template<class T, class... Args>
SharedPtr<T> makeShared(Args&& ... args) {
  return SharedPtr<T>(new typename SharedPtr<T>::template MakeControlBlock<T>(1, 0, std::forward<Args>(args)...));
}

template<class T, class Alloc, class... Args>
SharedPtr<T> allocateShared(Alloc& alloc, Args&& ... args) {
  using MakeCB = typename SharedPtr<T>::template MakeControlBlock<T, Alloc>;
  using BlockAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<MakeCB>;
  BlockAlloc block_alloc(alloc);
  auto ptr = block_alloc.allocate(1);
  std::allocator_traits<BlockAlloc>::construct(block_alloc, ptr, 1, 0, std::forward<Args>(args)...);
  return SharedPtr<T>(ptr);
}
