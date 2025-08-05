module;
#define TEMPLATE template <typename T, typename IT, std::size_t Max>
#define FIXED_POOL FixedPool<T, IT, Max>

module graphic.utils.fixed_pool;

TEMPLATE
FIXED_POOL::FixedPool() {
  free_list_.reserve(Max);
  for (int i = static_cast<int>(Max) - 1; i >= 0; --i) {
    free_list_.push_back(static_cast<Index>(i));
  }
}

TEMPLATE
std::expected<typename FIXED_POOL::Index, typename FIXED_POOL::InsertError>
FIXED_POOL::insert(const T& value) {
  if (free_list_.empty()) return std::unexpected(InsertError::POOL_FULL);

  Index idx = free_list_.back();
  free_list_.pop_back();

  data_[idx] = value;
  alive_[idx] = true;
  return idx;
}

TEMPLATE
void FIXED_POOL::remove(Index idx, const std::move_only_function<void(T&)>& func) {
  if (idx >= Max || !alive_[idx]) return;

  func(&data_);

  alive_[idx] = false;
  free_list_.push_back(idx);
}

TEMPLATE
T* FIXED_POOL::get(Index idx) {
  return idx < Max && alive_[idx] ? &data_[idx] : nullptr;
}

TEMPLATE
const T* FIXED_POOL::get(Index idx) const {
  return idx < Max && alive_[idx] ? &data_[idx] : nullptr;
}

TEMPLATE
bool FIXED_POOL::is_alive(Index idx) const {
  return idx < Max && alive_[idx];
}

TEMPLATE
void FIXED_POOL::update(Index idx, const std::move_only_function<void(T&)>& func) {
  if (idx >= Max || !alive_[idx]) return;

  func(data_[idx]);
}
