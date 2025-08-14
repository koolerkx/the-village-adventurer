module;

export module game.map.tile_repository;
import std;
import game.types;

export class TileRepository {
private:
  unsigned short texture_width_{}, texture_height_{}; // texture size
  unsigned short tile_width_{}, tile_height_{};
  unsigned short columns_count_{};
  unsigned int tileCount{};

  std::unordered_map<unsigned int, TileAnimationData> tile_animated_data_;
  std::unordered_map<unsigned int, std::vector<CollisionData>> tile_collision_data_;

public:
  TileRepository(std::string_view metadata_filepath);

  inline TileUV GetUvById(unsigned int tileId) const {
    return TileUV{
      .u = static_cast<unsigned short>(((tileId - 1) % columns_count_) * tile_width_),
      .v = static_cast<unsigned short>(((tileId - 1) / columns_count_) * tile_height_),
      .w = tile_width_,
      .h = tile_height_
    };
  }

  inline std::optional<TileAnimationData> GetTileAnimatedData(unsigned int tileId) const {
    if (auto it = tile_animated_data_.find(tileId); it != tile_animated_data_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  inline std::optional<std::vector<CollisionData>> GetTileCollisionData(unsigned int tileId) const {
    if (auto it = tile_collision_data_.find(tileId); it != tile_collision_data_.end()) {
      return it->second;
    }
    return std::nullopt;
  }
};
