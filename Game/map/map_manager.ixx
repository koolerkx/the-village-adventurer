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

// export struct MapManagerProps {
//   FixedPoolIndexType texture_id;
// };

export class MapManager {
private:
  std::unordered_map<std::string, TileMap> maps;
  MapData Load(std::string_view filepath, TileRepository* tr);

  FixedPoolIndexType texture_id_;

  // std::vector<std::shared_ptr<TileMap>> tile_maps;
  std::shared_ptr<TileMap> active_map_ = nullptr;

public:
  MapManager(GameContext* ctx);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  TileMap* GetActiveMap() { return active_map_.get(); }

  std::vector<TileMapObjectProps> GetMobProps() {
    return active_map_->GetMobProps();
  }

  std::vector<TileMapObjectProps> GetActiveAreaProps() {
    return active_map_->GetActiveAreaProps();
  }

  CollideState GetCollideState() const { return active_map_->GetCollideState(); }
  void SetCollideState(CollideState state) { active_map_->SetCollideState(state); }

  std::wstring GetMapName() const { return active_map_->GetMapName(); }
  Collider<TileMap> GetMapCollider() const { return active_map_->GetMapCollider(); }


  std::span<Collider<FieldObject>> GetFiledObjectColliders() {
    return active_map_->GetFiledObjectColliders();
  }
};
