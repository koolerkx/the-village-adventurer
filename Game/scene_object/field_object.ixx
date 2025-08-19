module;

export module game.map.field_object;

import std;
import game.collision.collider;
import game.types;
import game.scene_object;

export enum class FieldObjectType: unsigned char {
  NONE,
  WALL,
  CHEST
};

export struct FieldObject {
  Vector2 position;
  Tile tile;
  TileAnimationState animation_state{};
  Collider<FieldObject> collider{};
  TileMetaData metadata{};
  FieldObjectType type = FieldObjectType::NONE;
};

export void OnPlayerEnterFieldObject(FieldObject* field_object) {
  if (field_object->type == FieldObjectType::WALL) {
    std::cout << "WALL HIT" << std::endl;
  }
  else if (field_object->type == FieldObjectType::CHEST) {
    field_object->animation_state.is_playing = true;
    field_object->animation_state.current_frame_time += field_object->animation_state.frame_durations[0];
  }
};
