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

export enum class PlayerState: unsigned char {
  IDLE_LEFT,
  IDLE_RIGHT,
  IDLE_UP,
  IDLE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN
};

std::unordered_map<PlayerState, scene_object::AnimationFrameData> animation_data{
  {
    PlayerState::IDLE_LEFT, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 32),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::IDLE_RIGHT, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 32 * 2),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::IDLE_UP, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 0),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::IDLE_DOWN, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 32 * 3),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::MOVE_LEFT, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3, 0, 32),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  },
  {
    PlayerState::MOVE_RIGHT, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3, 0, 32 * 2),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  },
  {
    PlayerState::MOVE_UP, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  },
  {
    PlayerState::MOVE_DOWN, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3, 0, 32 * 3),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  }
};

export class Player {
private:
  FixedPoolIndexType texture_id_ = 0;

  scene_object::AnimationState animation_state_{};

  Transform transform_ = Transform{
    {0, 0, 0},
    {128, 128},
    {1, 1}
  };
  UV uv_{
    {0, 0},
    {32, 32}
  };

  PlayerState state_;

  COLOR color_ = color::white;
  CollisionData collision_{};

  Vector2 direction_;
  Vector2 velocity_;

  float move_speed_ = 300.0f; // px per second

  void UpdateState();
  void UpdateAnimation(float delta_time);

public:
  Player(GameContext* ctx);

  void SetState(PlayerState state);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx);
};
