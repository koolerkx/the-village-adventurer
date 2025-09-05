module;

export module game.map.field_object;

import std;
import game.collision.collider;
import game.types;
import game.scene_object;

export import game.field_object.chest;

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

export void OnPlayerEnterFieldObject(FieldObject* field_object,
                                     const std::function<void(FieldObject&)>& on_collide_wall = [](FieldObject&) {},
                                     const std::function<void(FieldObject&)>& on_chest_open = [](FieldObject&) {}
) {
  if (field_object->type == FieldObjectType::WALL) {
    on_collide_wall(*field_object);
    return;
  };

  // FieldObjectType::CHEST
  if (!field_object->animation_state.is_playing && field_object->animation_state.current_frame == 0) {
    field_object->animation_state.is_playing = true;
    field_object->animation_state.current_frame_time += field_object->animation_state.frame_durations[0];

    // On Chest Open
    on_chest_open(*field_object);
  }
};
