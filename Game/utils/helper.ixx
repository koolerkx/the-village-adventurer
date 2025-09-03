export module game.utils.helper;

import std;

export namespace helper {
  inline std::mt19937& GetRNG() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
  }
  
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
    
    std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);

    return vec[dist(GetRNG())];
  }

  float GetRandomNumberByRange(float from, float to) {
    std::uniform_real_distribution<float> dist(from, to);
    return dist(GetRNG());
  }

  float GetRandomNumberByOffset(float base, float offset) {
    std::uniform_real_distribution<float> dist(base - offset, base + offset);
    return dist(GetRNG());
  }
}
