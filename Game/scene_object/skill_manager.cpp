module;
#include <cassert>

module game.scene_object.skill;

const std::wstring texture_path = L"assets/attack.png"; // TODO: extract

SkillManager::SkillManager(GameContext* ctx) {
  scene_object::LoadTexture(texture_id, texture_path, ctx->render_resource_manager->texture_manager.get());\
}

void SkillManager::OnUpdate(GameContext* ctx, float delta_time) {}
void SkillManager::OnFixedUpdate(GameContext* ctx, float delta_time) {}
void SkillManager::OnRender(GameContext* ctx, Camera* camera) {}

void SkillManager::PlaySkill(SKILL_TYPE type, Vector2 position, float rotation) {
  auto result = skill_data.find(type);
  if (result == skill_data.end()) { return; }
  auto& data = result->second;

  auto transform = Transform{
    .position = {position.x, position.y, 0},
    .size = {static_cast<float>(data.frames[0].w), static_cast<float>(data.frames[0].h)},
    .scale = {1, 1},
    .rotation_radian = rotation,
    .rotation_pivot = {},
    .position_anchor = {static_cast<float>(-data.frames[0].w / 2), static_cast<float>(-data.frames[0].h / 2), 0},
  };

  auto collider = Collider<SkillHitbox>{
    .is_trigger = true,
    .position = {
      transform.position.x + transform.position_anchor.x, transform.position.y + transform.position_anchor.y
    },
    .rotation = 0,
    .owner = nullptr,
    .shape = RectCollider{
      0, 0, static_cast<float>(data.frames[0].w), static_cast<float>(data.frames[0].h)
    }
  };

  SkillHitbox hitbox = {
    .transform = transform,
    .collider = collider,
    .current_frame = 0,
    .current_frame_time = 0,
    .data = &data
  };

  auto insert_result = skill_pool_.Insert(std::move(hitbox));
  if (!insert_result.has_value()) {
    assert(false);
  }
  auto inserted = skill_pool_.Get(insert_result.value());
  inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
}
