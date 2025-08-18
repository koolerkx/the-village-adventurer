module;

export module game.map;

import std;
import graphic.utils.types;
import game.context;
import game.types;
import game.map.tile_repository;
import game.scene_object.camera;
import game.collision.collider;
import game.collision.object_pool;

static constexpr std::size_t MAX_WALL_COUNT = 1024; // TODO: extract

struct MapTile {
  std::vector<unsigned int> x{}; // 0 ~ map_width * tile_width
  std::vector<unsigned int> y{}; // 0 ~ (map_width - 1) * tile_height
  std::vector<unsigned int> u{}; // 0 ~ map_width * tile_width
  std::vector<unsigned int> v{}; // 0 ~ map_width * tile_width
  std::vector<int> tile_id{};
};

struct MapLayer {
  MapTile tiles;
  // index -> animation state
  std::unordered_map<unsigned int, TileAnimationState> tile_animation_states_;
};

// placeholder struct
struct Wall {};

export class TileMap {
private:
  unsigned int tile_width_{16};  // px
  unsigned int tile_height_{16}; // px
  unsigned int map_width_{16};   // blocks
  unsigned int map_height_{16};  // blocks
  std::vector<MapLayer> layers_;

  Transform transform_{};

  FixedPoolIndexType texture_id_{0};

  ObjectPool<Collider<Wall>, MAX_WALL_COUNT> wall_collider_;

public:
  TileMap();
  void Load(std::string_view filepath, FixedPoolIndexType texture_id, TileRepository* tr);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void SetTransform(const Transform& t) { transform_ = t; }
  Transform GetTransform() const { return transform_; }

  std::span<Collider<Wall>> GetWallColliders() { return wall_collider_.GetAll(); }
};
