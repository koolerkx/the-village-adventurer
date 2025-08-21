module;

module game.ui.game_ui;

import std;

GameUI::GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path) {
  texture_id_ = ctx->render_resource_manager->texture_manager->Load(texture_path);
}

void GameUI::OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {}

void GameUI::OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {}

void GameUI::OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera) {}
