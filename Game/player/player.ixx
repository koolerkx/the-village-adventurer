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
import game.scene_object.skill;
import game.player.buff;
import game.player.level;

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

export const std::vector<SKILL_TYPE> AVAILABLE_SKILLS = {
  SKILL_TYPE::NORMAL_ATTACK,
  SKILL_TYPE::FIREBALL
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
  static constexpr UV DEFAULT_UV{
    {0, 0},
    {32, 32}
  };

  static constexpr float DEFAULT_DEFENSE = 5.0f;
#pragma endregion

  Transform transform_ = DEFAULT_TRANSFORM;
  Transform transform_before_ = transform_;
  UV uv_ = DEFAULT_UV;
  Collider<Player> collider_{};

  PlayerState state_;
  std::unordered_map<PlayerState, scene_object::AnimationFrameData> animation_data_;

  COLOR color_ = color::white;
  float invincible_timer_ = 10.0f;
  int invincible_color_idx_ = 0;
  float invincible_color_switch_timer_ = 0.0f;
  const std::vector<COLOR> invincible_color_lists_ = {
    color::red300, color::pink300, color::purple300, color::deepPurple300, color::indigo300, color::blue300,
    color::lightBlue300, color::cyan300, color::teal300, color::green300, color::lightGreen300, color::lime300,
    color::yellow300, color::amber300, color::orange300, color::deepOrange300, color::brown300, color::grey300,
    color::blueGrey300
  };

  Vector2 direction_;
  Vector2 direction_facing_ = {0, 1}; // default facing down
  Vector2 velocity_;

  float move_speed_ = 100.0f;

  void UpdateState();
  void UpdateAnimation(float delta_time);

  std::unique_ptr<IPlayerInput> input_;
  Throttle attack_throttle_{0.3f};
  Throttle skill_select_throttle_{0.3f};
  Throttle function_key_throttle_{0.3f};

  int selected_skill_id_ = 0;
  SKILL_TYPE selected_skill_type_ = AVAILABLE_SKILLS[selected_skill_id_];

  // Game data
  float health_ = 100.0f;
  float max_health_ = 100.0f;

  float defense = DEFAULT_DEFENSE;

  // buffs
  std::vector<PlayerBuff> buffs_;

  // level up ability
  std::vector<player_level::PlayerAbility> level_up_abilities_;

  // experience
  int experience_ = 0;
  int total_experience_ = 0;
  int level_ = 1;
  int max_experience_ = 80;

public:
  void SetState(PlayerState state);

  Vector2 GetPositionVector() const { return {transform_.position.x, transform_.position.y}; }
  Vector2 GetSizeVector() const { return {transform_.size.x, transform_.size.y}; }

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

  float Damage(float amount) {
    float damage = amount - defense
      * player_level::GetLevelAbilityMultiplier(level_up_abilities_, player_level::Ability::DEFENSE)
      + player_level::GetLevelAbilityValue(level_up_abilities_, player_level::Ability::DEFENSE);

    health_ = std::max(health_ - damage, 0.0f);
    return health_;
  }

  float Heal(float amount) {
    health_ = std::min(health_ + amount, max_health_);
    SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::buff1, {}, 0.5);
    return health_;
  }

  float GetHPPercentage() const { return health_ / max_health_; }

  Player(FixedPoolIndexType texture_id, std::unique_ptr<IPlayerInput> input,
         std::unordered_map<PlayerState, scene_object::AnimationFrameData> anim_data);
  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);

  int GetSelectedSkillId() const { return selected_skill_id_; }

  void AddBuff(PlayerBuff pb) {
    if (pb.type == BuffType::INVINCIBLE) {
      std::erase_if(buffs_, [](const PlayerBuff b) { return b.type == BuffType::INVINCIBLE; });
    }
    SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::buff2, {}, 0.5);
    buffs_.push_back(pb);
  }

  std::vector<PlayerBuff> GetBuffs() { return buffs_; }

  bool GetIsInvincible() const {
    for (auto b : buffs_) {
      if (b.type == BuffType::INVINCIBLE) return true;
    }
    return false;
  }

  void AddExperience(int value) {
    experience_ += value;
    total_experience_ += value;
    if (experience_ >= max_experience_) {
      level_++;
      experience_ -= max_experience_;
      SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::level_up, {}, 1.0f);
    }
  }

  float GetExperiencePercentage() const {
    return static_cast<float>(experience_) / static_cast<float>(max_experience_);
  }

  int GetLevel() const { return level_; }

  void AddLevelUpAbility(player_level::PlayerAbility ability) {
    level_up_abilities_.push_back(ability);
    SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::buff2, {}, 0.5);
  };

  float GetMaxHp() const { return max_health_; }

  std::vector<player_level::PlayerAbility> GetLevelUpAbilities() { return level_up_abilities_; }
};
