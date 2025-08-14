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
  std::vector<TileUV> frames;
  std::vector<float> frame_durations;
};

export struct CollisionData {
  unsigned int x;
  unsigned int y;
  unsigned int width;
  unsigned int height;
  bool is_circle = false;
};