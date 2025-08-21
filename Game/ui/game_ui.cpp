module;

module game.ui.game_ui;

import std;
import graphic.utils.types;
import graphic.utils.font;

GameUI::GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path) {
  texture_id_ = ctx->render_resource_manager->texture_manager->Load(texture_path);

  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);
}

void GameUI::OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {}

void GameUI::OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {}

void GameUI::OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera) {
  auto& rr = ctx->render_resource_manager->renderer;
  std::wstringstream wss;

  std::vector<RenderInstanceItem> render_items;

  // Session: Left Upper
  // HP Bar: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {24, 24, 0},
      .size = {320, 64},
    },
    texture_map["HPBarPanel"], color::white
  });
  // HP Bar: Heart Background Circle
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {28, 20, 0},
      .size = {72, 72},
    },
    texture_map["BigCircle"], color::white
  });
  // HP Bar: Heart
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {38, 30, 0},
      .size = {52, 52},
    },
    texture_map["Heart"], color::white
  });
  // HP Bar: HB Bar
  float hp_percent = 1.0f;
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {110, 59, 0},
      .size = {200 / hp_percent, 14},
    },
    texture_map["HPBar"], color::white
  });
  // HP Bar: HB Bar Frame
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {106, 54, 0},
      .size = {208, 24},
    },
    texture_map["HPBarFrame"], color::white
  });
  // On Top of instanced Draw
  // Session: Left Upper
  // HP Bar: Player Name
  rr->DrawFont(L"プレイヤー", font_key_,
               Transform{.position = {110, 36, 0}},
               StringSpriteProps{
                 .pixel_size = 14.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f
               });
}
