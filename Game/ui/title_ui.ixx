module;
#include "stdint.h"

export module game.ui.title_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;
import game.types;

import game.ui.component.input_hint;

export class TitleUI {
private:
  std::wstring font_key_;
  Font* default_font_;

  FixedPoolIndexType title_texture_id_;
  FixedPoolIndexType background_1_texture_id_;
  FixedPoolIndexType background_2_texture_id_;
  FixedPoolIndexType background_3_texture_id_;
  FixedPoolIndexType background_4_texture_id_;
  FixedPoolIndexType ui_texture_id_;
  FixedPoolIndexType overlay_texture_id_;
  FixedPoolIndexType criware_logo_texture_id_;

  const Vector2 frame_uv_pos_ = {224, 146};
  const Vector2 frame_uv_size_ = {64, 16};

  const Vector2 selected_uv_pos_ = {384, 117};
  const Vector2 selected_uv_size_ = {60, 23};

  float uv_offset_speed_ = 0.0f;
  const float uv_offset_speed_target_ = 25.0f;
  float uv_horizontal_offset_ = 0;

  float title_logo_floating_speed_ = 18.0f;
  float movement_acc_ = 0.0f;

  const float selected_frame_moving_speed_ = 2.0f;
  const float selected_frame_moving_range_ = 13.0f;
  uint8_t selected_option_ = 0;

  COLOR fade_overlay_color_ = color::white;
  float fade_overlay_alpha_current_ = 1.0f;
  float fade_overlay_alpha_target_ = 0.0f;

  std::function<void()> fade_overlay_callback_ = {};

  bool is_x_input_ = false;
  std::unique_ptr<InputHintComponent> input_hint_ = nullptr;
  std::unique_ptr<InputHintComponent> x_button_input_hints_ = nullptr;

  float opacity_ = 0.0f;

public:
  TitleUI(GameContext* ctx, bool is_flash_start);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void SetSelectedOption(uint8_t option) {
    selected_option_ = option;
  }

  void SetFadeOverlayAlphaTarget(float alpha, COLOR color, std::function<void()> cb = {}) {
    fade_overlay_alpha_target_ = alpha;
    fade_overlay_color_ = color;

    if (std::fabs(fade_overlay_alpha_target_ - fade_overlay_alpha_current_) < 0.01f) {
      if (cb) cb();
      fade_overlay_callback_ = {};
    }
    else {
      fade_overlay_callback_ = cb;
    }
  }

  void SetIsXInput(bool is_x_input) { is_x_input_ = is_x_input; }
};
