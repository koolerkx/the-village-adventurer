module;

export module game.scene_object.player;

import std;
import graphic.utils.fixed_pool;
import graphic.utils.types;
import graphic.utils.color;
import game.types;
import game.scene_object;

export struct Player {
  FixedPoolIndexType texture_id = 0;

  // Texture data
  static constexpr std::wstring_view texture_path = L"assets/character_01.png"; // TODO: extract

  scene_object::AnimationState animation_state{
    .is_loop = true,
    .play_on_start = true,
    .is_playing = true,
    .frames = scene_object::MakeFramesVector(3, 32, 32, 3),
    .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    .current_frame = 0,
    .current_frame_time = 0
  };

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
