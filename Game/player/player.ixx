module;

export module game.player;

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
import game.player.input;
import game.utils.throttle;

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

export class Player {
private:
  FixedPoolIndexType texture_id_ = 0;

  scene_object::AnimationState animation_state_{};

#pragma region PLAYER_DEFAULT_VALUE
  static constexpr Transform DEFAULT_TRANSFORM{
    .position = {0, 0, 0},
    .size = {16, 16},
    .scale = {1, 1},
    .position_anchor = {-8, -8, 0}
  };

  static constexpr CollisionData COLLISION_DATA{
    .x = 0,
    .y = 0,
    .width = 16,
    .height = 16,
    .is_circle = false, // rect
  };
  static constexpr float COLLIDER_PADDING = 2.0f;
  static constexpr UV DEFAULT_UV {
    {0, 0},
    {32, 32}
  };
#pragma endregion
  
  Transform transform_ = DEFAULT_TRANSFORM;
  Transform transform_before_ = transform_;
  UV uv_ = DEFAULT_UV;
  Collider<Player> collider_{};

  PlayerState state_;
  std::unordered_map<PlayerState, scene_object::AnimationFrameData> animation_data_;

  COLOR color_ = color::white;

  Vector2 direction_;
  Vector2 direction_facing_ = {0, 1}; // default facing down
  Vector2 velocity_;

  float move_speed_ = 125.0f;

  void UpdateState();
  void UpdateAnimation(float delta_time);

  std::unique_ptr<IPlayerInput> input_;
  Throttle attack_throttle_{0.3f};
  Throttle function_key_throttle_{0.3f};

  // Game data
  float health_ = 100.0f;
  float max_health_ = 100.0f;

public:
  void SetState(PlayerState state);

  Vector2 GetPositionVector() const { return {transform_.position.x, transform_.position.y}; }

  Collider<Player> GetCollider() const {
    return collider_;
  }

  Transform GetTransform() const { return transform_; }

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

  void Damage(float amount) { health_ = std::max(health_ - amount, 0.0f); }
  void Heal(float amount) { health_ = std::min(health_ + amount, max_health_); }
  float GetHPPercentage() const { return health_ / max_health_; }

  Player(FixedPoolIndexType texture_id, std::unique_ptr<IPlayerInput> input, std::unordered_map<PlayerState, scene_object::AnimationFrameData> anim_data);
  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
};