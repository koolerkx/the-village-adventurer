module;

module game.scene.splash_scene;

import std;
import game.ui.interpolation;
import graphic.utils.types;
import game.scene_manager;
import game.title_scene;

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

void SplashScene::OnUpdate(GameContext*, float delta_time) {
  if (recommendation_timeout_ > 0.1) {
    recommendation_opacity_ = interpolation::UpdateSmoothValue(
      recommendation_opacity_,
      1,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.5
    );
  }
  if (recommendation_timeout_ > 0 && recommendation_opacity_ > 0.95) {
    recommendation_timeout_ -= delta_time;
  }
  if (recommendation_timeout_ < 0) {
    recommendation_opacity_ = interpolation::UpdateSmoothValue(
      recommendation_opacity_,
      0,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.5
    );
    overlay_opacity_ = interpolation::UpdateSmoothValue(
      overlay_opacity_,
      0,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.5
    );
  }

  if (overlay_opacity_ <= 0.9) {
    if (swirl_twists_ > 0) {
      swirl_twists_ = interpolation::UpdateSmoothValue(
        swirl_twists_,
        0,
        delta_time,
        interpolation::SmoothType::EaseOut,
        0.5
      );
    }
    else {
      swirl_twists_ = 0;
    }
  }

  if (swirl_twists_ <= 0.05 && name_timeout_ >= 0.95) {
    name_opacity_ = interpolation::UpdateSmoothValue(
      name_opacity_,
      1,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.5
    );
  }
  if (name_opacity_ >= 0.95) {
    name_timeout_ -= delta_time;
  }
  if (name_timeout_ < 0) {
    name_opacity_ -= 0.5f * delta_time;
  }
  if (name_timeout_ < 0 && name_opacity_ < 0.05) {
    SceneManager::GetInstance().ChangeSceneDelayed(std::make_unique<TitleScene>(false, false));
  }
}

void SplashScene::OnFixedUpdate(GameContext*, float) {}

void SplashScene::OnRender(GameContext* ctx) {
  auto& rr = ctx->render_resource_manager->renderer;

  // Background
  rr->DrawSprite(RenderItem{
                   ui_texture_id_,
                   Transform{
                     {0, 0, 0},
                     {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
                   },
                   {{96, 297}, {8, 8}},
                   color::black
                 }, {}, PSType::Swirl);

  rr->SetSwirlParams({0.25, 0.5}, 0.5, swirl_twists_, {0.75, 0.5}, 0.5, -swirl_twists_);

  rr->DrawSprite(RenderItem{

                   background_1_texture_id_,
                   Transform{
                     {0, 0, 0},
                     {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
                   },
                   {{uv_horizontal_offset_, 0}, {576, 324}},
                   color::white,
                 }, {},
                 PSType::Swirl);

  rr->DrawSprite(RenderItem{
                   background_2_texture_id_,
                   Transform{
                     {0, 0, 0},
                     {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
                   },
                   {{uv_horizontal_offset_ * 0.6f, 0}, {576, 324}},
                   color::white
                 }, {}, PSType::Swirl);

  rr->DrawSprite(RenderItem{
                   background_3_texture_id_,
                   Transform{
                     {0, 0, 0},
                     {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
                   },
                   {{uv_horizontal_offset_ * 0.8f, 0}, {576, 324}},
                   color::white
                 }, {}, PSType::Swirl);

  rr->DrawSprite(RenderItem{
                   background_4_texture_id_,
                   Transform{
                     {0, 0, 0},
                     {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
                   },
                   {{uv_horizontal_offset_ * 1.2f, 0}, {576, 324}},
                   color::white
                 }, {}, PSType::Swirl);

  std::wstringstream wss;
  wss << "KOOLER FAN" << std::endl;

  StringSpriteSize size_name = default_font_->GetStringSize(wss.str(), {}, {.pixel_size = 48.0f});

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {
                   ctx->window_width / 2 - size_name.width / 2,
                   ctx->window_height / 2 - size_name.height / 2,
                   0
                 }
               }, {.pixel_size = 48.0f, .color = color::setOpacity(color::white, name_opacity_)}, {});

  wss.str(L"");
  wss << "presents" << std::endl;

  StringSpriteSize size_presents = default_font_->GetStringSize(wss.str(), {}, {.pixel_size = 48.0f});

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {
                   ctx->window_width / 2 - size_presents.width / 2,
                   ctx->window_height / 2 - size_presents.height / 2 + size_name.height / 2,
                   0
                 }
               }, {.pixel_size = 48.0f, .color = color::setOpacity(color::white, name_opacity_)}, {});


  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{96, 297}, {8, 8}},
    color::setOpacity(color::black, overlay_opacity_)
  });

  // Recommendation
  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {304, 248, 0},
      {128, 128}
    },
    {{452, 170}, {32, 32}},
    color::setOpacity(color::white, overlay_opacity_ * recommendation_opacity_)
  });

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {793, 248, 0},
      {128, 128}
    },
    {{452, 202}, {32, 32}},
    color::setOpacity(color::white, overlay_opacity_ * recommendation_opacity_)
  });

  rr->DrawFont(L"　ステレオ対応\nヘッドセット推奨", font_key_, {
                 .position = {
                   224, 388, 0
                 }
               }, {
                 .pixel_size = 36.0f, .line_spacing = 12.0f,
                 .color = color::setOpacity(color::white, recommendation_opacity_)
               }, {});


  rr->DrawFont(L"　ゲームパッド最適化\nキーボード操作にも対応", font_key_, {
                 .position = {
                   659, 388, 0
                 }
               }, {
                 .pixel_size = 36.0f, .line_spacing = 12.0f,
                 .color = color::setOpacity(color::white, recommendation_opacity_)
               }, {});
}

void SplashScene::OnExit(GameContext*) {}
