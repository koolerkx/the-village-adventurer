module;
#include "stdint.h"

export module game.scene_object.skill;

import graphic.utils.types;
import graphic.utils.fixed_pool;
import game.collision.collider;
import game.types;
import game.scene_object;
import game.object_pool;
import game.context;
import game.scene_object.camera;

import std;

export enum class SKILL_TYPE: uint8_t {
  NORMAL_ATTACK,
  FIREBALL,
  EXPLOSION,
  NONE
};

export struct SkillData {
  std::wstring name;
  SKILL_TYPE type;
  float cooldown;
  short damage;
  bool is_stick_to_player = false;
  bool is_loop = false;
  bool is_destroy_by_wall = false;
  bool is_destroy_by_mob = false;

  Transform base_transform;
  Padding base_collider_padding; // Top, Right, Bottom, Left

  std::vector<TileUV> frames;
  std::vector<float> frame_durations;
  Vector2 moving_speed = {0, 0}; // normalized speed
  SKILL_TYPE spawn_next = SKILL_TYPE::NONE;
  UV icon_uv = {{0, 0}, {100, 100}};
};

export struct SkillHitbox {
  Transform transform;
  UV uv;
  Collider<SkillHitbox> collider;
  // animation
  size_t current_frame = 0;
  float current_frame_time = 0.f;
  bool is_playing = true;
  bool is_destroy_on_next = false; // one-way flag for removal

  const SkillData* data = nullptr;
  std::unordered_set<ObjectPoolIndexType> hit_mobs{}; // hit each mob only once
};

export const std::unordered_map<SKILL_TYPE, SkillData> skill_data = {
  {
    SKILL_TYPE::NORMAL_ATTACK, {
      .name = L"ŽaŒ‚",
      .cooldown = 1.0f,
      .damage = 10,
      .is_stick_to_player = true,
      .base_transform = Transform{
        .size = {36, 36},
        .scale = {1, 1},
        .rotation_radian = 90.0f * static_cast<float>(std::numbers::pi) / 180.0f,
        .rotation_pivot = {0, 2, 0},
        .position_anchor = {-18, -20, 0},
      },
      .base_collider_padding = {6, 8, 18, 8},
      .frames = scene_object::MakeFramesVector(5, 128, 128, 10, 960, 4512),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.04f, 10),
      .icon_uv = {{16, 811}, {16, 16}}
    },
  },
  {
    SKILL_TYPE::FIREBALL, {
      .name = L"‰Î‚Ì‹Ê",
      .cooldown = 1.0f,
      .damage = 5,
      .is_stick_to_player = false,
      .is_loop = true,
      .is_destroy_by_wall = true,
      .is_destroy_by_mob = true,
      .base_transform = Transform{
        .size = {24, 24},
        .scale = {1, 1},
        .rotation_radian = 0.0f * static_cast<float>(std::numbers::pi) / 180.0f,
        .rotation_pivot = {0, 0, 0},
        .position_anchor = {-12, -12, 0}, // FIXME: position calculation offset
      },
      .base_collider_padding = {12, 0, 5, 2},
      .frames = scene_object::MakeFramesVector(4, 48, 48, 4, 960, 6048),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 4),
      .moving_speed = {175, 175},
      .spawn_next = SKILL_TYPE::EXPLOSION,
      .icon_uv = {{48, 779}, {16, 16}}
    },
  },
  {
    SKILL_TYPE::EXPLOSION, {
      .name = L"‰Î‚Ì‹Ê(”š”­)",
      .cooldown = 1.0f,
      .damage = 2,
      .is_stick_to_player = false,
      .is_loop = false,
      .is_destroy_by_wall = false,
      .base_transform = Transform{
        .size = {32, 32},
        .scale = {1, 1},
        .rotation_radian = 0.0f * static_cast<float>(std::numbers::pi) / 180.0f,
        .rotation_pivot = {0, 0, 0},
        .position_anchor = {-12, -12, 0},
      },
      .base_collider_padding = {4, 4, 4, 4},
      .frames = scene_object::MakeFramesVector(12, 48, 48, 12, 1248, 6096),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 12),
      .moving_speed = {0, 0},
    },
  }
};

export class SkillManager {
private:
  FixedPoolIndexType texture_id;
  ObjectPool<SkillHitbox> hitbox_pool_;

public:
  SkillManager(GameContext* ctx);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time, Transform player_transform);
  void OnRender(GameContext* ctx, Camera* camera);

  void PlaySkill(SKILL_TYPE type, Vector2 position, float rotation = 0);

  void HandleDestroyCollision(SkillHitbox* skill, std::optional<Vector2> next_position = std::nullopt);

  std::vector<Collider<SkillHitbox>> GetColliders();
};
