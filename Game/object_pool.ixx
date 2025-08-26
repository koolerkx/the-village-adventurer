module;

export module game.object_pool;

import std;

export using ObjectPoolIndexType = unsigned __int16;

constexpr std::size_t FALLBACK_MAX = 65535;

export template <typename T,
                 typename IT = ObjectPoolIndexType,
                 std::size_t Max = (std::numeric_limits<IT>::max() < FALLBACK_MAX
                                      ? std::numeric_limits<IT>::max()
                                      : FALLBACK_MAX)>
class ObjectPool {
  using Index = IT;

private:
  std::unique_ptr<T[]> data_;     // heap array for objects
  std::unique_ptr<bool[]> alive_; // heap array for flags
  std::vector<Index> free_list_;
  std::vector<Index> alive_indices_;

  enum class InsertError { POOL_FULL };

public:
  ObjectPool()
    : data_(std::make_unique<T[]>(Max)),
      alive_(std::make_unique<bool[]>(Max)) {
    free_list_.reserve(Max);
    alive_indices_.reserve(Max);

    std::fill_n(alive_.get(), Max, false);

    for (int i = static_cast<int>(Max) - 1; i >= 0; --i) {
      free_list_.push_back(static_cast<Index>(i));
    }
  }

  std::expected<Index, InsertError> Insert(const T& value) {
    if (free_list_.empty()) return std::unexpected(InsertError::POOL_FULL);

    Index idx = free_list_.back();
    free_list_.pop_back();

    data_[idx] = value;
    alive_[idx] = true;
    alive_indices_.push_back(idx);
    return idx;
  }

  std::expected<Index, InsertError> Insert(T&& value) {
    if (free_list_.empty()) return std::unexpected(InsertError::POOL_FULL);

    Index idx = free_list_.back();
    free_list_.pop_back();

    data_[idx] = std::move(value);
    alive_[idx] = true;
    alive_indices_.push_back(idx);
    return idx;
  }

  void Remove(Index idx, std::move_only_function<void(T&)> func = [](T&) {}) {
    if (idx >= Max || !alive_[idx]) return;

    func(data_[idx]);

    alive_[idx] = false;
    free_list_.push_back(idx);

    auto it = std::ranges::find(alive_indices_, idx);
    if (it != alive_indices_.end()) {
      std::iter_swap(it, alive_indices_.end() - 1);
      alive_indices_.pop_back();
    }
  }

  template <typename Func>
  void RemoveIf(Func&& fn) {
    for (int i = static_cast<int>(alive_indices_.size()) - 1; i >= 0; --i) {
      Index idx = alive_indices_[i];
      T& item = data_[idx];

      if constexpr (std::is_invocable_v<Func, T&>) {
        if (fn(item)) {
          alive_indices_.erase(alive_indices_.begin() + i);
          free_list_.push_back(idx);
          alive_[idx] = false;
        }
      }
      else if constexpr (std::is_invocable_v<Func, T&, Index>) {
        if (fn(item, idx)) {
          alive_indices_.erase(alive_indices_.begin() + i);
          free_list_.push_back(idx);
          alive_[idx] = false;
        }
      }
    }
  }

  T* Get(Index idx) {
    return idx < Max && alive_[idx] ? &data_[idx] : nullptr;
  }

  const T* Get(Index idx) const {
    return idx < Max && alive_[idx] ? &data_[idx] : nullptr;
  }

  bool IsAlive(Index idx) const {
    return idx < Max && alive_[idx];
  }

  void Update(Index idx, const std::move_only_function<void(T&)>& func) {
    if (idx >= Max || !alive_[idx]) return;
    func(data_[idx]);
  }

  std::size_t Size() const {
    return alive_indices_.size();
  }

  template <typename Func>
  void ForEach(Func&& fn) {
    for (Index idx : alive_indices_) {
      if constexpr (std::is_invocable_v<Func, T&>) {
        fn(data_[idx]);
      }
      else if constexpr (std::is_invocable_v<Func, T&, Index>) {
        fn(data_[idx], idx);
      }
    }
  }

  std::span<const Index> AliveIndices() const {
    return {alive_indices_.data(), alive_indices_.size()};
  }
};
