module;

export module game.map.tilemap_object_handler;

import std;
import game.types;

export enum class TileMapObjectType: char {
  MOB_SLIME,
  MOB_SLIME_GREEN,
  MOB_DUMMY,
  TRIGGER_ACTIVE_AREA,
  NONE
};

export struct TileMapObjectProps {
  float x;
  float y;
  float width;
  float height;
  TileMapObjectType type;
};

export namespace tilemap_object_handler {
  using ::TileMapObjectType;

  TileMapObjectType MapTileMapObject(std::string_view type, std::string_view name) {
    if (type == "trigger_area" && name == "active_area") {
      return TileMapObjectType::TRIGGER_ACTIVE_AREA;
    }
    if (type == "slime") {
      return TileMapObjectType::MOB_SLIME;
    }
    if (type == "slime_green") {
      return TileMapObjectType::MOB_SLIME_GREEN;
    }
    if (type == "dummy") {
      return TileMapObjectType::MOB_DUMMY;
    }
    return TileMapObjectType::NONE;
  }

  std::vector<TileMapObjectProps> GetMobProps(std::vector<TileMapObjectProps> props, Vector2 offset_position) {
    return props
      | std::views::filter([](const auto& it) {
        return it.type == TileMapObjectType::MOB_SLIME
          || it.type == TileMapObjectType::MOB_SLIME_GREEN
          || it.type == TileMapObjectType::MOB_DUMMY;
      })
      | std::views::transform([offset_position](auto& it) {
        it.x += offset_position.x;
        it.y += offset_position.y;
        return it;
      })
      | std::ranges::to<std::vector>();
  }

  std::vector<TileMapObjectProps> GetActiveAreaProps(std::vector<TileMapObjectProps> props, Vector2 offset_position) {
    return props
      | std::views::filter([](const auto& it) { return it.type == TileMapObjectType::TRIGGER_ACTIVE_AREA; })
      | std::views::transform([offset_position](auto& it) {
        it.x += offset_position.x;
        it.y += offset_position.y;
        return it;
      })
      | std::ranges::to<std::vector>();
  }
}
