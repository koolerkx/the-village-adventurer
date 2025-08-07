module;

export module graphic.utils.fixed_pool;

import std;

export using FixedPoolIndexType = unsigned __int8;

constexpr std::size_t FALLBACK_MAX = 65535;

export template <typename T,
                 typename IT = FixedPoolIndexType,
                 std::size_t Max = (std::numeric_limits<IT>::max() < FALLBACK_MAX ? std::numeric_limits<IT>::max() : FALLBACK_MAX)>
class FixedPool {
  using Index = IT;

private:
  std::array<T, Max> data_{};
  std::array<bool, Max> alive_{false};
  std::vector<Index> free_list_;

  enum class InsertError {
    POOL_FULL
  };

public:
  FixedPool() {
    free_list_.reserve(Max);
    for (int i = static_cast<int>(Max) - 1; i >= 0; --i) {
      free_list_.push_back(static_cast<Index>(i));
    }
  }

  std::expected<Index, InsertError> insert(const T& value) {
    if (free_list_.empty()) return std::unexpected(InsertError::POOL_FULL);

    Index idx = free_list_.back();
    free_list_.pop_back();

    data_[idx] = value;
    alive_[idx] = true;
    return idx;
  }

  void remove(Index idx, std::move_only_function<void(T&)> func = [](T&) {}) {
    if (idx >= Max || !alive_[idx]) return;

    func(data_[idx]);

    alive_[idx] = false;
    free_list_.push_back(idx);
  }

  T* get(Index idx) {
    return idx < Max && alive_[idx] ? &data_[idx] : nullptr;
  }

  const T* get(Index idx) const {
    return idx < Max && alive_[idx] ? &data_[idx] : nullptr;
  }

  bool is_alive(Index idx) const {
    return idx < Max && alive_[idx];
  }

  void update(Index idx, const std::move_only_function<void(T&)>& func) {
    if (idx >= Max || !alive_[idx]) return;

    func(data_[idx]);
  }
};
