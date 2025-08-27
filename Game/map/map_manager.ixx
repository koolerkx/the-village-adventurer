module;
#include <assert.h>

export module game.map.map_manager;

import std;
import game.context;
import game.scene_object.camera;
import game.types;

import game.map;
import game.map.tile_repository;
import game.map.tilemap_object_handler;
import game.map.linked_map;

import game.collision.collider;
import game.map.field_object;

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

  std::vector<std::shared_ptr<LinkedMapNode>> map_nodes;
  std::vector<std::shared_ptr<TileMap>> tile_maps;
  std::weak_ptr<LinkedMapNode> active_map_node_;

  void ExpandMap(std::shared_ptr<LinkedMapNode> map);

public:
  MapManager(GameContext* ctx);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  std::shared_ptr<TileMap> GetActiveMap();
  std::vector<std::shared_ptr<TileMap>> GetActiveMaps();
  std::vector<std::shared_ptr<LinkedMapNode>> GetActiveLinkedMaps();
  void ForEachActiveLinkedMapsNode(std::function<void(std::shared_ptr<LinkedMapNode>)> fn);

  std::vector<TileMapObjectProps> GetMobProps() {
    std::vector<TileMapObjectProps> v1{};
    for (auto map : GetActiveMaps()) {
      auto v2 = map->GetMobProps();
      v1.insert(v1.end(), v2.begin(), v2.end());
    }
    return v1;
  }

  std::vector<TileMapObjectProps> GetActiveAreaProps() {
    std::vector<TileMapObjectProps> v1{};
    for (auto map : GetActiveMaps()) {
      auto v2 = map->GetActiveAreaProps();
      v1.insert(v1.end(), v2.begin(), v2.end());
    }
    return v1;
  }

  std::wstring GetMapName() const {
    if (auto node = active_map_node_.lock())
      if (auto map = node->data.lock())
        return map->GetMapName();
    assert(false);
    return L"";
  }

  std::vector<Collider<FieldObject>> GetFiledObjectColliders() {
    std::vector<Collider<FieldObject>> v1{};
    for (auto map : GetActiveMaps()) {
      auto v2 = map->GetFiledObjectColliders();
      v1.insert(v1.end(), v2.begin(), v2.end());
    }
    return v1;
  }

  void EnterNewMap(std::shared_ptr<LinkedMapNode> node);
};
