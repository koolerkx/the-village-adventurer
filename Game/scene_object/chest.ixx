module;

export module game.map.chest;

import std;
import game.collision.collider;
import game.types;
import game.scene_object;

export struct Chest {};

export struct FieldObject {
  Vector2 position;
  Tile tile;
  TileAnimationState animation_state_{};
  Collider<FieldObject> collider;

  std::variant<Chest> type;
};