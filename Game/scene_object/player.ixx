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
import game.scene_object.camera;
import game.collision.collider;
import game.map;
import game.scene_game.context;
import game.map.field_object;

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
    .position = {0, 0, 0},
    .size = {16, 16},
    .scale = {1, 1},
    .position_anchor = {-8, -8, 0}
  };
  Transform transform_before_ = transform_;

  UV uv_{
    {0, 0},
    {32, 32}
  };

  static constexpr CollisionData COLLISION_DATA{
    .x = 0,
    .y = 0,
    .width = 16,
    .height = 16,
    .is_circle = false, // rect
  };
  static constexpr float COLLIDER_PADDING = 2.0f;
  Collider<Player> collider_{};

  PlayerState state_;

  COLOR color_ = color::white;

  Vector2 direction_;
  Vector2 velocity_;

  float move_speed_ = 125.0f; // px per second

  void UpdateState();
  void UpdateAnimation(float delta_time);

public:
  void SetState(PlayerState state);

  Vector2 GetPositionVector() const { return {transform_.position.x, transform_.position.y}; }

  Collider<Player> GetCollider() const {
    return collider_;
  }

  void SetTransform(std::function<void(Transform&)> func) {
    transform_before_ = transform_;

    collider_.position.x -= (transform_.position.x + transform_.position_anchor.x);
    collider_.position.y -= (transform_.position.y + transform_.position_anchor.y);

    func(transform_);

    collider_.position.x = transform_.position.x + transform_.position_anchor.x;
    collider_.position.y = transform_.position.y + transform_.position_anchor.y;
  }

  void ResetTransform() {
    transform_ = transform_before_;
  }

  void SetCollider(std::function<void(Collider<Player>&)> func) {
    func(collider_);
  }

  Vector2 GetVelocity() const;

  Player(GameContext* ctx, SceneContext* scene_ctx);
  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
};
