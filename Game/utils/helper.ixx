export module game.utils.helper;

import std;

export namespace helper {
  template <typename K, typename V>
  std::vector<K> GetKeysFromMap(const std::unordered_map<K, V>& map) {
    std::vector<K> keys;
    keys.reserve(map.size());
    for (const auto& [key, val] : map) {
      keys.push_back(key);
    }
    return keys;
  }

  template <typename T>
  const T& GetRandomElement(const std::vector<T>& vec) {
    if (vec.empty()) {
      throw std::runtime_error("Vector is empty");
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);

    return vec[dist(gen)];
  }
}
