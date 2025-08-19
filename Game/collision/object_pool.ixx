module;
#include <cassert>

export module game.collision.object_pool;
import game.collision.collider;
import game.map.field_object;

import std;

export template <std::size_t MaxSize>
struct FieldObjectPool {
  std::array<FieldObject, MaxSize> objects;
  size_t size = 0;

  FieldObject* Add(const FieldObject& obj) {
    assert(size < MaxSize && "ObjectPool overflow");
    objects[size] = obj;
    objects[size].collider.owner = &objects[size];
    return &objects[size++];
  }

  void Clear() { size = 0; }

  std::span<FieldObject> GetAll() { return std::span(objects.begin(), size); }

  std::span<Collider<FieldObject>> GetAllCollider() {
    static std::array<Collider<FieldObject>, MaxSize> colliders;
    for (size_t i = 0; i < size; ++i) {
      colliders[i] = objects[i].collider;
    }
    return std::span<Collider<FieldObject>>(colliders.data(), size);
  }

  template <typename Func>
  void EditAll(Func&& fn) {
    for (size_t i = 0; i < size; ++i) {
      fn(objects[i]);
    }
  }
};
