module;

export module game.map.tile_repository;
import std;

struct UV {
  unsigned int v;
  unsigned int u;
  unsigned short w = 16;
  unsigned short h = 16;
};

struct Tile {
  UV uv;
};

struct TileAnimatedData {
  bool is_loop = false;
  bool play_on_start = false;
  std::vector<UV> frames;
  std::vector<float> frame_durations;
};

struct CollisionData {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  bool is_circle = false;
};

export class TileRepository {
  unsigned short texture_width_{}, texture_height_{}; // texture size
  unsigned short tile_width_{}, tile_height_{};
  unsigned short columns_count_{};
  unsigned int tileCount{};

  std::map<unsigned int, TileAnimatedData> tile_animated_data_;
  std::map<unsigned int, std::vector<CollisionData>> tile_collision_data_;

  inline UV GetUvById(unsigned int tileId) const {
    return UV{
      .u = static_cast<unsigned short>((tileId % columns_count_) * tile_width_),
      .v = static_cast<unsigned short>((tileId / columns_count_) * tile_height_),
      .w = tile_width_,
      .h = tile_height_
    };
  }

  inline std::optional<TileAnimatedData> GetTileAnimatedData(unsigned int tileId) const {
    return tile_animated_data_.contains(tileId) ? tile_animated_data_.at(tileId) : std::nullopt;
  }

  inline std::optional<std::vector<CollisionData>> GetTileCollisionData(unsigned int tileId) const {
    return tile_collision_data_.contains(tileId) ? tile_collision_data_.at(tileId) : std::nullopt;
  }
};
