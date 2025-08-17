module;

export module game.scene_object.player;

import std;
import graphic.utils.fixed_pool;
import graphic.utils.types;
import graphic.utils.color;
import game.input;
import game.scene_manager;
import game.types;
import game.scene_object;

export class Player {
private:
  FixedPoolIndexType texture_id_ = 0;

  scene_object::AnimationState animation_state_{
    .is_loop = true,
    .play_on_start = true,
    .is_playing = true,
    .frames = scene_object::MakeFramesVector(3, 32, 32, 3),
    .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    .current_frame = 0,
    .current_frame_time = 0
  };

  Transform transform_ = Transform{
    {0, 0, 0},
    {128, 128},
    {1, 1}
  };
  UV uv_{
    {0, 0},
    {32, 32}
  };

  COLOR color_ = color::white;
  CollisionData collision_{};

  Vector2 direction_;
  Vector2 velocity_;

  float move_speed_ = 100.0f; // px per second

  void UpdateAnimation(float delta_time);

public:
  Player(GameContext* ctx);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx);
};
