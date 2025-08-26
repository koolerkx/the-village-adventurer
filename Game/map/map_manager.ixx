module;

export module game.map.map_manager;

import std;
import game.context;
import game.scene_object.camera;
import game.types;

import game.map;
import game.map.tile_repository;
import game.map.tilemap_object_handler;

import game.collision.collider;
import game.map.field_object;
#include <assert.h>

// export struct MapManagerProps {
//   FixedPoolIndexType texture_id;
// };

export class MapManager {
private:
  std::unordered_map<std::string, std::vector<std::unique_ptr<MapData>>> map_data_preloaded_;
  float map_width_px_;
  float map_height_px_;

  MapData Load(std::string_view filepath, TileRepository* tr);

  FixedPoolIndexType texture_id_;

  std::vector<std::shared_ptr<TileMap>> tile_maps;
  std::weak_ptr<TileMap> active_map_;

public:
  MapManager(GameContext* ctx);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  std::shared_ptr<TileMap> GetActiveMap() {
    if (auto map = active_map_.lock())
      return map;
    return nullptr;
  }

  std::vector<TileMapObjectProps> GetMobProps() {
    if (auto map = active_map_.lock())
      return map->GetMobProps();
    assert(false);
  }

  std::vector<TileMapObjectProps> GetActiveAreaProps() {
    if (auto map = active_map_.lock())
      return map->GetActiveAreaProps();
    assert(false);
  }

  CollideState GetCollideState() const {
    if (auto map = active_map_.lock())
      return map->GetCollideState();
    assert(false);
  }

  void SetCollideState(CollideState state) {
    if (auto map = active_map_.lock())
      return map->SetCollideState(state);
    assert(false);
  }

  std::wstring GetMapName() const {
    if (auto map = active_map_.lock())
      return map->GetMapName();
    assert(false);
  }

  Collider<TileMap> GetMapCollider() const {
    if (auto map = active_map_.lock())
      return map->GetMapCollider();
    assert(false);
  }


  std::span<Collider<FieldObject>> GetFiledObjectColliders() {
    if (auto map = active_map_.lock())
      return map->GetFiledObjectColliders();
    assert(false);
  }
};
