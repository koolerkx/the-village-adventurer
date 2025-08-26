module;
#include "tinyxml/tinyxml2.h"
export module game.map;

import std;
import graphic.utils.types;
import game.context;
import game.types;
import game.map.tile_repository;
import game.scene_object.camera;
import game.collision.collider;
import game.map.field_object;
import game.object_pool;
import game.map.tilemap_object_handler;

constexpr int MAX_WALL_COUNT = 4096; 

export struct MapTile {
  std::vector<unsigned int> x{}; // 0 ~ map_width * tile_width
  std::vector<unsigned int> y{}; // 0 ~ (map_width - 1) * tile_height
  std::vector<unsigned int> u{}; // 0 ~ map_width * tile_width
  std::vector<unsigned int> v{}; // 0 ~ map_width * tile_width
  std::vector<int> tile_id{};
};

export struct MapLayer {
  MapTile tiles; ///< Display only tile
  // index -> animation state
  std::unordered_map<unsigned int, TileAnimationState> tile_animation_states_;
};

export struct MapData {
  std::wstring map_name = L"";
  unsigned int tile_width = 16;  // px
  unsigned int tile_height = 16; // px
  unsigned int map_width = 16;   // blocks
  unsigned int map_height = 16;  // blocks

  // Note: Collider not support scaling
  ObjectPool<FieldObject> field_object_pool{}; // wall and interactable object
  std::vector<MapLayer> layers;
  std::vector<TileMapObjectProps> map_objects_props;
};

export class TileMap {
private:
  // MapData;
  std::wstring map_name_ = L"";
  unsigned int tile_width_ = 16;  // px
  unsigned int tile_height_ = 16; // px
  unsigned int map_width_ = 16;   // blocks
  unsigned int map_height_ = 16;  // blocks

  ObjectPool<FieldObject> field_object_pool_{}; // wall and interactable object
  std::vector<MapLayer> layers_;
  std::vector<TileMapObjectProps> map_objects_props_;
  
  Transform transform_{};

  FixedPoolIndexType texture_id_;

  Collider<TileMap> map_collider_;
  CollideState collide_state_ = CollideState::NOT_COLLIDE;

public:
  TileMap(MapData* map_data, FixedPoolIndexType texture_id, Vector2 base_position);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void SetTransform(const Transform& t) {
    field_object_pool_.ForEach([&](FieldObject& field_object) {
      std::visit([&]<typename Shape>(Shape&) {
        if constexpr (std::is_same_v<Shape, RectCollider> || std::is_same_v<Shape, CircleCollider>) {
          field_object.position.x -= transform_.position.x;
          field_object.position.y -= transform_.position.y;
          field_object.collider.position.x -= transform_.position.x;
          field_object.collider.position.y -= transform_.position.y;
        }
      }, field_object.collider.shape);
    });

    transform_ = t;

    field_object_pool_.ForEach([&](FieldObject& field_object) {
      std::visit([&]<typename Shape>(Shape&) {
        if constexpr (std::is_same_v<Shape, RectCollider> || std::is_same_v<Shape, CircleCollider>) {
          field_object.position.x += transform_.position.x;
          field_object.position.y += transform_.position.y;
          field_object.collider.position.x += transform_.position.x;
          field_object.collider.position.y += transform_.position.y;
        }
      }, field_object.collider.shape);
    });
  }

  Transform GetTransform() const { return transform_; }

  std::span<Collider<FieldObject>> GetFiledObjectColliders() {
    static std::vector<Collider<FieldObject>> colliders;
    colliders.clear();
    colliders.reserve(MAX_WALL_COUNT);

    field_object_pool_.ForEach([&](FieldObject& it) {
      colliders.push_back(it.collider);
    });
    return colliders;
  }

  std::vector<TileMapObjectProps> GetMobProps() {
    return tilemap_object_handler::GetMobProps(map_objects_props_, {transform_.position.x, transform_.position.y});
  }

  std::vector<TileMapObjectProps> GetActiveAreaProps() {
    return tilemap_object_handler::GetActiveAreaProps(map_objects_props_, {transform_.position.x, transform_.position.y});
  }

  CollideState GetCollideState() const { return collide_state_; }
  void SetCollideState(CollideState state) { collide_state_ = state; }

  std::wstring GetMapName() const { return map_name_; }
  Collider<TileMap> GetMapCollider() const { return map_collider_; }
};
