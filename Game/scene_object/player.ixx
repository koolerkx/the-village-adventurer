module;

export module game.scene_object.player;

import std;
import graphic.utils.fixed_pool;
import graphic.utils.types;
import graphic.utils.color;
import game.types;

export struct Player {
  FixedPoolIndexType texture_id = 0;
  static constexpr std::wstring_view texture_path = L"assets/character_01.png"; // TODO: extract

  Transform transform = Transform{
    {0, 0, 0},
    {128, 128},
    {1, 1}
  };
  UV uv{
    {0, 0},
    {32, 32}
  };

  COLOR color = color::white;
  CollisionData collision{};
};
