module;
#include "stdint.h"

export module game.ui.level_up;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;
import game.types;
import game.utils.helper;
import game.player.level;

// kamifubuki64 animation data
constexpr int frame = 36;
constexpr int frame_per_line = 3;
constexpr UV start_uv = {{0, 0}, {640, 480}};
constexpr float timeout_per_frame = 0.05f;

struct LevelUpOptionData {
  UV uv;
  std::wstring text;
};

export class LevelUpUI {
private:
  std::wstring font_key_;
  Font* default_font_;

  FixedPoolIndexType background_texture_id_;
  FixedPoolIndexType kamifubuki_texture_id_;
  FixedPoolIndexType ui_texture_id_;

  int current_kamifubuki_ = 0;
  float kamifubuki_frame_timeout = timeout_per_frame;

  const Vector2 frame_uv_pos_ = {224, 146};
  const Vector2 frame_uv_size_ = {64, 16};

  const Vector2 selected_uv_pos_ = {384, 117};
  const Vector2 selected_uv_size_ = {60, 23};

  const Vector2 option_frame_uv_pos_ = {0, 227};
  const Vector2 option_frame_uv_size_ = {96, 96};

  const float selected_frame_moving_speed_ = 2.0f;
  const float selected_frame_moving_range_ = 13.0f;
  uint8_t selected_option_ = 0;

  float title_logo_floating_speed_ = 18.0f;
  float movement_acc_ = 0.0f;

  std::array<LevelUpOptionData, 3> options_ = {
    LevelUpOptionData{{{48, 827}, {16, 16}}, L"çUåÇóÕUP"},
    LevelUpOptionData{{{0, 811}, {16, 16}}, L"à⁄ìÆë¨ìxUP"},
    LevelUpOptionData{{{0, 811}, {16, 16}}, L"ñhå‰óÕUP"}
  };

  float opacity_current_ = 1.0f;
  float opacity_target_ = 1.0f;

  std::function<void()> fade_in_callback_ = {};

public:
  LevelUpUI(GameContext* ctx);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void SetSelectedOption(uint8_t option) {
    selected_option_ = option;
  }

  void Reset() {
    selected_option_ = 1;
    movement_acc_ = 0.0f;
  }

  void SetOptionData(std::array<player_level::OptionType, 3> elems) {
    for (size_t i = 0; i < options_.size(); ++i) {
      options_[i] = LevelUpOptionData{
        player_level::GetOptionUV(elems[i]),
        player_level::GetOptionDescription(elems[i])
      };
    }
  }

  void SetFadeInWithCallback(const std::function<void()>& cb = {}) {
    opacity_current_ = 0.0f;
    fade_in_callback_ = cb;
  }
};
