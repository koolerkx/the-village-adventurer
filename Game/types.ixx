module;

export module game.types;

import std;

export struct TileUV {
  unsigned int u;
  unsigned int v;
  unsigned short w = 16;
  unsigned short h = 16;
};

export struct Tile {
  TileUV uv;
};

// the tile animation pattern metadata
export struct TileAnimationData {
  bool is_loop = false;
  bool play_on_start = false;
  std::vector<TileUV> frames;
  std::vector<float> frame_durations;
};

// the tile animation state in map
export struct TileAnimationState {
  bool is_loop = false;
  bool play_on_start = false;
  bool is_playing = false;
  std::vector<TileUV> frames;
  std::vector<float> frame_durations;

  size_t current_frame = 0;
  float current_frame_time = 0.f;
};

export struct TileMetaData {
  std::string tile_class;
};

export struct CollisionData {
  int x;
  int y;
  unsigned int width;
  unsigned int height;
  bool is_circle = false;
};

export struct Vector2 {
  float x = .0f;
  float y = .0f;
};

export struct Padding {
  float top = .0f;
  float right = .0f;
  float bottom = .0f;
  float left = .0f;
};

export enum class CollideState {
  COLLIDING,
  NOT_COLLIDE,
  COLLIDE_LAST_FRAME
};