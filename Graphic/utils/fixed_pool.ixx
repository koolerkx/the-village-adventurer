module;

export module graphic.utils.fixed_pool;

import std;

export using FixedPoolIndexType = unsigned __int8;

export template <typename T,
                 typename IT = FixedPoolIndexType,
                 std::size_t Max = std::numeric_limits<IT>::max()>
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
  FixedPool();

  std::expected<Index, InsertError> insert(const T& value);
  void remove(Index idx, const std::move_only_function<void(T&)>& func = [](T&) {});
  T* get(Index idx);
  const T* get(Index idx) const;
  bool is_alive(Index idx) const;
  void update(Index idx, const std::move_only_function<void(T&)>& func);
};
