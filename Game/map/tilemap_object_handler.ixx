module;

export module game.map.tilemap_object_handler;

import std;

export enum class TileMapObjectType: char {
  MOB_SLIME,
  TRIGGER_ACTIVE_AREA,
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
  }
}