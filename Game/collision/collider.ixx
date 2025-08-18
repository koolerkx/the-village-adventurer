module;

export module game.collision.collider;

import std;
import graphic.utils.types;
import game.types;

export struct RectCollider {
  float x, y, width, height;
};

export struct CircleCollider {
  float x, y, radius;
};

export using ColliderShape = std::variant<RectCollider, CircleCollider>;

export template <typename Owner>
struct Collider {
  bool is_trigger = false;
  Vector2 position;
  float rotation;
  Owner* owner;
  ColliderShape shape;
};
