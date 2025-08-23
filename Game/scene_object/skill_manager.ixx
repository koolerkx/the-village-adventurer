module;

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

struct SkillData {
  std::string name;
  float cooldown;
  short damage;
  bool is_stick_to_player = false;

  Transform base_transform;
  Padding base_collider_padding; // Top, Right, Bottom, Left

  std::vector<TileUV> frames;
  std::vector<float> frame_durations;
  std::vector<Vector2> frame_displacement;
};

export struct SkillHitbox {
  Transform transform;
  UV uv;
  Collider<SkillHitbox> collider;
  // animation
  size_t current_frame = 0;
  float current_frame_time = 0.f;
  bool is_playing = true;

  const SkillData* data = nullptr;
  std::unordered_set<ObjectPoolIndexType> hit_mobs {}; // hit each mob only once
};

export enum class SKILL_TYPE {
  NORMAL_ATTACK
};

const std::unordered_map<SKILL_TYPE, SkillData> skill_data = {
  {
    SKILL_TYPE::NORMAL_ATTACK, {
      .name = "Slash 1",
      .cooldown = 1.0f,
      .damage = 3,
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
    }
  }
};

export class SkillManager {
private:
  FixedPoolIndexType texture_id;
  ObjectPool<SkillHitbox> hitbox_pool_;

public:
  SkillManager(GameContext* ctx);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera, Transform player_transform);

  void PlaySkill(SKILL_TYPE type, Vector2 position, float rotation = 0);

  std::vector<Collider<SkillHitbox>> GetColliders();
};
