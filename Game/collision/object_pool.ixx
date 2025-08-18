module;
#include <cassert>

export module game.collision.object_pool;

import std;

export template <typename T, std::size_t MaxSize>
struct ObjectPool {
  std::array<T, MaxSize> objects;
  size_t size = 0;

  T* Add(const T& obj) {
    assert(size < MaxSize && "ObjectPool overflow");
    objects[size] = obj;
    return &objects[size++];
  }

  void Clear() { size = 0; }

  std::span<T> GetAll() { return std::span(objects.begin(), size); }

  template <typename Func>
  void EditAll(Func&& fn) {
    for (size_t i = 0; i < size; ++i) {
      fn(objects[i]);
    }
  }
};
