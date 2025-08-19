module;

module game.scene_object.skill;

const std::wstring texture_path = L"assets/attack.png"; // TODO: extract
const std::vector<SkillData> skill_data = {
  {
    .name = "Normal Attack",
    .cooldown = 1.0f,
    .frames = scene_object::MakeFramesVector(9, 120, 160, 9, 2688, 0),
    .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 1),
  }
};

SkillManager::SkillManager(GameContext* ctx) {
  scene_object::LoadTexture(texture_id, texture_path, ctx->render_resource_manager->texture_manager.get());

  skill_data_ = skill_data;
}
