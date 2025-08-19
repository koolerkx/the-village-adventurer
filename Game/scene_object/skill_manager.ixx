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
  
  std::vector<TileUV> frames;
  std::vector<float> frame_durations;
  std::vector<Vector2> frame_displacement;
};

export struct SkillHitbox {
  Transform transform;
  Collider<SkillHitbox> collider;

  // animation
  size_t current_frame = 0;
  float current_frame_time = 0.f;

  const SkillData* data = nullptr;
};

enum class SKILL_TYPE {
  NORMAL_ATTACK,
};

const std::unordered_map<SKILL_TYPE, SkillData> skill_data = {
  {
    SKILL_TYPE::NORMAL_ATTACK, {
      .name = "Normal Attack",
      .cooldown = 1.0f,
      .frames = scene_object::MakeFramesVector(9, 120, 160, 9, 2688, 0),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 1),
    }
  }
};

export class SkillManager {
private:
  FixedPoolIndexType texture_id;
  ObjectPool<SkillHitbox> skill_pool_;
  
public:
  SkillManager(GameContext* ctx);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void PlaySkill(SKILL_TYPE type, Vector2 position, float rotation = 0);
};
