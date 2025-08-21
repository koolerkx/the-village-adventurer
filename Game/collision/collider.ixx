module;

export module game.collision.collider;

import std;
import graphic.utils.types;
import game.types;

export struct RectCollider {
  float x, y, width, height;
  float base_width = 16.0f, base_height = 16.0f; // used for rotation 
};

export struct CircleCollider {
  float x, y, radius;
};

export using ColliderShape = std::variant<RectCollider, CircleCollider>;

export template <typename Owner>
struct Collider {
  bool is_trigger = false;
  Vector2 position; // item world position
  float rotation;
  Vector2 rotation_pivot{0, 0}; // center-center
  Owner* owner;
  ColliderShape shape; // offset
};
