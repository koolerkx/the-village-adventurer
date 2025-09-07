module;

module game.scene.splash_scene;

import graphic.utils.types;

SplashScene::SplashScene() {}

void SplashScene::OnEnter(GameContext* ctx) {
  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  auto& tm = ctx->render_resource_manager->texture_manager;

  background_1_texture_id_ = tm->Load(L"assets/title_bg_1.png");
  background_2_texture_id_ = tm->Load(L"assets/title_bg_2.png");
  background_3_texture_id_ = tm->Load(L"assets/title_bg_3.png");
  background_4_texture_id_ = tm->Load(L"assets/title_bg_4.png");
  ui_texture_id_ = tm->Load(L"assets/ui.png");
}

void SplashScene::OnUpdate(GameContext* ctx, float delta_time) {
  uv_horizontal_offset_ += delta_time * uv_offset_speed_;
}

void SplashScene::OnFixedUpdate(GameContext* ctx, float delta_time) {}

void SplashScene::OnRender(GameContext* ctx) {
  auto& rr = ctx->render_resource_manager->renderer;

  rr->DrawSprite(RenderItem{
    background_1_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_, 0}, {576, 324}},
    color::white
  });

  rr->DrawSprite(RenderItem{
    background_2_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_ * 0.6f, 0}, {576, 324}},
    color::white
  });

  rr->DrawSprite(RenderItem{
    background_3_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_ * 0.8f, 0}, {576, 324}},
    color::white
  });

  rr->DrawSprite(RenderItem{
    background_4_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_ * 1.2f, 0}, {576, 324}},
    color::white
  });
}

void SplashScene::OnExit(GameContext* ctx) {}
