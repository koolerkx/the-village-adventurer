module;
#include <WinString.h>

module game.title_scene;

import std;
import graphic.utils.types;

void TitleScene::OnEnter(GameContext* ctx) {
  OutputDebugString("TitleScene> OnEnter\n");

  texture_id = ctx->render_resource_manager->texture_manager->Load(L"assets/block_test.png", "test");
}

void TitleScene::OnUpdate(GameContext*, float delta_time) {
  OutputDebugString("TitleScene> OnUpdate\n");
}

void TitleScene::OnRender(GameContext* ctx) {
  Transform transform1 = {
    .position = POSITION(100.0f, 100.0f, 0.0f),
    .size = {100.0f, 100.0f},
    .rotation_radian = 45.0f * static_cast<float>(std::numbers::pi) / 180.0f,
  };
  UV uv1 = {{0, 0}, {8, 8}};
  COLOR color1 = {1.0f, 1.0f, 1.0f, 1.0f};

  // ctx->render_resource_manager->renderer->DrawSprite(texture_id, transform1, uv1, color1);


  OutputDebugString("TitleScene> OnRender\n");
}

void TitleScene::OnExit(GameContext*) {
  OutputDebugString("TitleScene> OnExit\n");
}
