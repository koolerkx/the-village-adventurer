module;

module game.ui.title_ui;

import std;
import game.ui.interpolation;

TitleUI::TitleUI(GameContext* ctx, bool is_flash_start) {
  if (!is_flash_start) {
    fade_overlay_alpha_current_ = 0;
  }

  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  auto& tm = ctx->render_resource_manager->texture_manager;

  title_texture_id_ = tm->Load(L"assets/title.png");
  background_1_texture_id_ = tm->Load(L"assets/title_bg_1.png");
  background_2_texture_id_ = tm->Load(L"assets/title_bg_2.png");
  background_3_texture_id_ = tm->Load(L"assets/title_bg_3.png");
  background_4_texture_id_ = tm->Load(L"assets/title_bg_4.png");
  ui_texture_id_ = tm->Load(L"assets/ui.png");
  overlay_texture_id_ = tm->Load(L"assets/block_white.png");
  criware_logo_texture_id_ = tm->Load(L"assets/criware_logo01.png");
  logo_texture_id_ = tm->Load(L"assets/app.ico");

  std::vector<InputHint> input_hints = {
    InputHint{L"確認", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
    InputHint{L"選択", {KeyCode::KK_W, KeyCode::KK_S, KeyCode::KK_UP, KK_DOWN}}
  };
  std::vector<InputHint> x_button_input_hints = {
    InputHint{L"確認", {XButtonCode::A}},
    InputHint{L"選択", {XButtonCode::LeftThumb, XButtonCode::DPadUp, XButtonCode::DPadDown}},
  };

  input_hint_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                       1, input_hints, false, true
                                                     });
  x_button_input_hints_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                 1, x_button_input_hints, false, true
                                                               });

  std::vector<InputHint> input_hints_for_credit = {
    InputHint{L"戻る", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
    InputHint{L"スクロール", {KeyCode::KK_W, KeyCode::KK_S, KeyCode::KK_UP, KK_DOWN}}
  };
  std::vector<InputHint> x_button_input_hints_for_credit = {
    InputHint{L"戻る", {XButtonCode::A}},
    InputHint{L"スクロール", {XButtonCode::LeftThumb, XButtonCode::DPadUp, XButtonCode::DPadDown}},
  };

  input_hint_for_credit_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                  1, input_hints_for_credit, false, true
                                                                });
  x_button_input_hints_for_credit_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                            1, x_button_input_hints_for_credit, false,
                                                                            true
                                                                          });
}

void TitleUI::OnUpdate(GameContext* ctx, float delta_time) {
  uv_offset_speed_ = interpolation::UpdateSmoothValue(
    uv_offset_speed_,
    uv_offset_speed_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    1.0f
  );

  uv_horizontal_offset_ += delta_time * uv_offset_speed_;

  movement_acc_ += delta_time;

  fade_overlay_alpha_current_ = interpolation::UpdateSmoothValue(
    fade_overlay_alpha_current_,
    fade_overlay_alpha_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    1.0f
  );

  if (fade_overlay_callback_) {
    float diff = std::fabs(fade_overlay_alpha_target_ - fade_overlay_alpha_current_);
    if (diff < 0.01f) {
      auto cb = fade_overlay_callback_;
      fade_overlay_callback_ = {};
      cb();
    }
  }

  opacity_ = interpolation::UpdateSmoothValue(
    opacity_,
    1.0f,
    delta_time,
    interpolation::SmoothType::Linear,
    1.0f
  );

  input_hint_->OnUpdate(ctx, delta_time);
  x_button_input_hints_->OnUpdate(ctx, delta_time);
  input_hint_for_credit_->OnUpdate(ctx, delta_time);
  x_button_input_hints_for_credit_->OnUpdate(ctx, delta_time);
}

void TitleUI::OnFixedUpdate(GameContext* ctx, float delta_time) {
  input_hint_->OnFixedUpdate(ctx, delta_time);
  x_button_input_hints_->OnFixedUpdate(ctx, delta_time);
  input_hint_for_credit_->OnFixedUpdate(ctx, delta_time);
  x_button_input_hints_for_credit_->OnFixedUpdate(ctx, delta_time);
}

void TitleUI::OnRender(GameContext* ctx, Camera*) {
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
  });

  rr->DrawSprite(RenderItem{
    title_texture_id_,
    Transform{
      {
        static_cast<float>(ctx->window_width) / 2 - 225,
        static_cast<float>(ctx->window_height) / 2 - 300 +
        std::cos(1.5f * movement_acc_) * title_logo_floating_speed_,
        0
      },
      {450, 300}
    },
    {{0, 0}, {300, 200}},
    color::setOpacity(color::white, opacity_)
  });

  // Start Button
  float start_button_center_x = static_cast<float>(ctx->window_width) / 2;
  float start_button_center_y = static_cast<float>(ctx->window_height) / 2 + 50;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        start_button_center_x - 256 / 2,
        start_button_center_y - 64 / 2,
        0
      },
      {256, 64},
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::setOpacity(color::white, opacity_)
  });

  auto start_text_props = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  auto start_text_size = default_font_->GetStringSize(L"ゲームスタート", {}, start_text_props);

  rr->DrawFont(L"ゲームスタート",
               font_key_,
               Transform{
                 .position = {
                   start_button_center_x - start_text_size.width / 2,
                   start_button_center_y - start_text_size.height / 2,
                   0
                 }
               },
               start_text_props);

  // Credit Button
  float credit_button_center_x = static_cast<float>(ctx->window_width) / 2;
  float credit_button_center_y = static_cast<float>(ctx->window_height) / 2 + 136;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        credit_button_center_x - 256 / 2,
        credit_button_center_y - 64 / 2,
        0
      },
      {256, 64},
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::setOpacity(color::white, opacity_)
  });

  auto credit_text_props = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  auto credit_text_size = default_font_->GetStringSize(L"クレジット", {}, credit_text_props);

  rr->DrawFont(L"クレジット",
               font_key_,
               Transform{
                 .position = {
                   credit_button_center_x - credit_text_size.width / 2,
                   credit_button_center_y - credit_text_size.height / 2,
                   0
                 }
               },
               start_text_props);

  // End Button
  float end_button_center_x = static_cast<float>(ctx->window_width) / 2;
  float end_button_center_y = static_cast<float>(ctx->window_height) / 2 + 221;

  auto end_text_props = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        end_button_center_x - 256 / 2,
        end_button_center_y - 64 / 2,
        0
      },
      {256, 64}
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::setOpacity(color::white, opacity_)
  });

  auto end_text_size = default_font_->GetStringSize(L"ゲーム終了", {}, end_text_props);

  rr->DrawFont(L"ゲーム終了",
               font_key_,
               Transform{
                 .position = {
                   end_button_center_x - end_text_size.width / 2,
                   end_button_center_y - end_text_size.height / 2,
                   0
                 }
               },
               end_text_props);

  // Selected Frame
  float selected_frame_x = selected_option_ == 0
                             ? start_button_center_x
                             : selected_option_ == 1
                             ? credit_button_center_x
                             : end_button_center_x;
  float selected_frame_y = selected_option_ == 0
                             ? start_button_center_y
                             : selected_option_ == 1
                             ? credit_button_center_y
                             : end_button_center_y;

  float selected_width = 260 + selected_frame_moving_range_ * std::abs(
    std::cos(selected_frame_moving_speed_ * movement_acc_));
  float selected_height = 64 + selected_frame_moving_range_ * std::abs(
    std::cos(selected_frame_moving_speed_ * movement_acc_));
  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        selected_frame_x - selected_width / 2,
        selected_frame_y - selected_height / 2,
        0
      },
      {selected_width, selected_height},
    },
    {{selected_uv_pos_.x, selected_uv_pos_.y}, {selected_uv_size_.x, selected_uv_size_.y}},
    color::setOpacity(color::white, opacity_)
  });

  // Version
  std::wstring version_text = L"v0.1";
  auto version_text_size = default_font_->GetStringSize(version_text, {}, end_text_props);
  constexpr float version_text_padding = 10.0f;

  rr->DrawFont(version_text,
               font_key_,
               Transform{
                 .position = {
                   version_text_padding,
                   ctx->window_height - version_text_size.height - version_text_padding,
                   0
                 }
               },
               end_text_props);

  // CRIWARE Logo
  rr->DrawSprite(RenderItem{
    criware_logo_texture_id_,
    Transform{
      .position = {-80, -80, 0},
      .size = {64, 64},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0},
    },
    {{0, 0}, {1000, 1000}},
    color::setOpacity(color::white, opacity_)
  });

  if (is_x_input_) {
    x_button_input_hints_->OnRender(ctx);
  }
  else {
    input_hint_->OnRender(ctx);
  }

  // Credit
#pragma region Background Frame
  if (is_show_credit_) {
    std::vector<RenderInstanceItem> ui_render_instances = {};

#pragma region Background Overlay
    const Vector2 overlay_uv_pos_ = {96, 297};
    const Vector2 overlay_uv_size_ = {8, 8};

    constexpr float BACKGROUND_OVERLAY_OPACITY = 0.7f;
    ui_render_instances.emplace_back(RenderInstanceItem{
      .transform = {
        .position = {0, 0, 0},
        .size = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)},
        .position_anchor = {0, 0, 0}
      },
      .uv = {{overlay_uv_pos_.x, overlay_uv_pos_.y}, {overlay_uv_size_.x, overlay_uv_size_.y}},
      .color = color::setOpacity(color::black, opacity_ * BACKGROUND_OVERLAY_OPACITY)

    });
#pragma endregion

    const float win_center_x = ctx->window_width / 2.0f;
    const float win_center_y = ctx->window_height / 2.0f;

    constexpr float frame_width = 840;
    constexpr float frame_height = 480;
    constexpr float frame_padding = 16;
    constexpr float frame_content_width = frame_width - frame_padding * 2;
    constexpr float frame_content_height = frame_height - frame_padding * 2;
    const float frame_content_x = win_center_x - frame_content_width / 2;
    const float frame_content_y = win_center_y - frame_content_height / 2;

    const Vector2 background_uv_pos_ = {0, 875};
    const Vector2 background_uv_size_ = {840, 480};


    ui_render_instances.emplace_back(RenderInstanceItem{
      .transform = {
        .position = {-frame_width / 2, -frame_height / 2, 0},
        .size = {frame_width, frame_height},
        .position_anchor = {win_center_x, win_center_y, 0}
      },
      .uv = {{background_uv_pos_.x, background_uv_pos_.y}, {background_uv_size_.x, background_uv_size_.y}},
      .color = color::setOpacity(color::white, opacity_)
    });

    rr->DrawSpritesInstanced(ui_render_instances, ui_texture_id_, {}, true);
    ui_render_instances.clear();

    rr->SetScissorRect(frame_content_x, frame_content_y,
                       frame_content_x + frame_content_width, frame_content_y + frame_content_height);

    constexpr float logo_size = 128;
    constexpr float cri_logo_size = 64;

    auto credit_content_props = StringSpriteProps{
      .pixel_size = 24.0f,
      .letter_spacing = 0.0f,
      .line_height = 28.0f,
      .color = color::setOpacity(color::white, opacity_)
    };

    rr->DrawSprite(RenderItem{
      logo_texture_id_,
      Transform{
        .position = {win_center_x - logo_size / 2, win_center_y + credit_offset_y_, 0},
        .size = {logo_size, logo_size},
      },
      {{0, 0}, {96, 96}},
      color::setOpacity(color::white, opacity_)
    });

    rr->DrawSprite(RenderItem{
      criware_logo_texture_id_,
      Transform{
        .position = {
          win_center_x - cri_logo_size / 2, win_center_y + credit_offset_y_ + credit_content_props.line_height * 8, 0
        },
        .size = {cri_logo_size, cri_logo_size},
      },
      {{0, 0}, {1000, 1000}},
      color::setOpacity(color::white, opacity_)
    });

    std::wstringstream wss;
    wss.str(L"");
    wss << L"ゲーム《村の冒険者》　作者：KOOLER FAN\n\n";

    wss << L"音声\n\n\n\n\n";
    wss << L"このソフトウェアには、\n（株）ＣＲＩ・ミドルウェアの「CRIWARE」が使用されています。\n\n";
    wss << L"AlkaKrab https://alkakrab.itch.io/\n\n";
    wss << L"効果音ラボ https://soundeffect-lab.info/\n\n";

    wss << L"画像素材\n\n";
    wss << L"ぴぽや https://pipoya.net\n\n";
    wss << L"ドット絵世界 https://yms.main.jp\n\n";
    wss << L"白黒洋菓子店 https://noir-et-blanc-patisserie.amebaownd.com\n\n";
    wss << L"白螺子屋 https://hi79.web.fc2.com\n\n";
    wss << L"CraftPix.net https://craftpix.net\n\n";

    wss << L"Fantasy Minimal Pixel Art GUI by etahoshi\n";
    wss << L"https://etahoshi.itch.io/minimal-fantasy-gui-by-eta\n\n";

    wss << L"Free Smoke Fx Pixel 2\n";
    wss << L"https://bdragon1727.itch.io/free-smoke-fx-pixel-2\n\n";

    wss << L"Gamepad UI / Controller Prompts Pack\n";
    wss << L"https://greatdocbrown.itch.io/gamepad-ui\n\n";

    wss << L"2d Pixel Art Training Dummy Sprites\n";
    wss << L"https://elthen.itch.io/2d-pixel-art-training-dummy\n";


    rr->DrawFont(wss.str(),
                 font_key_,
                 Transform{
                   .position = {
                     frame_content_x + 16,
                     win_center_y + credit_offset_y_ + logo_size + 16,
                     0
                   }
                 },
                 end_text_props);

    rr->ResetScissorRect();
#pragma endregion

#pragma region back button
    constexpr float button_width = 200;
    constexpr float button_height = 50;
    constexpr float button_y_offset = 20;
    constexpr float button_margin_top = 16;

    float back_button_center_x = ctx->window_width / 2.0f;
    float back_button_center_y = frame_content_y + frame_content_height + frame_padding
      + button_margin_top + button_y_offset;
    wss.str(L"");
    wss << L"戻る";

    rr->DrawSprite(RenderItem{
      ui_texture_id_,
      Transform{
        {
          back_button_center_x - button_width / 2,
          back_button_center_y - button_height / 2,
          0
        },
        {button_width, button_height},
      },
      {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
      color::setOpacity(color::white, opacity_)
    });

    auto back_text_props = StringSpriteProps{
      .pixel_size = 24.0f,
      .letter_spacing = 0.0f,
      .line_height = 0.0f,
      .color = color::setOpacity(color::white, opacity_)
    };
    auto back_text_size = default_font_->GetStringSize(wss.str(), {}, back_text_props);

    rr->DrawFont(wss.str(),
                 font_key_,
                 Transform{
                   .position = {
                     back_button_center_x - back_text_size.width / 2,
                     back_button_center_y - back_text_size.height / 2,
                     0
                   }
                 },
                 back_text_props);

    constexpr float selection_frame_padding = 4.0f;
    float back_selected_frame_x = back_button_center_x;
    float back_selected_frame_y = back_button_center_y;

    float back_selected_width = button_width + selection_frame_padding + selected_frame_moving_range_ * std::abs(
      std::cos(selected_frame_moving_speed_ * movement_acc_));
    float back_selected_height = button_height + selection_frame_padding + selected_frame_moving_range_ * std::abs(
      std::cos(selected_frame_moving_speed_ * movement_acc_));
    rr->DrawSprite(RenderItem{
      ui_texture_id_,
      Transform{
        {
          back_selected_frame_x - back_selected_width / 2,
          back_selected_frame_y - back_selected_height / 2,
          0
        },
        {back_selected_width, back_selected_height},
      },
      {{selected_uv_pos_.x, selected_uv_pos_.y}, {selected_uv_size_.x, selected_uv_size_.y}},
      color::setOpacity(color::white, opacity_)
    });

    if (is_x_input_) {
      x_button_input_hints_for_credit_->OnRender(ctx);
    }
    else {
      input_hint_for_credit_->OnRender(ctx);
    }
  }
#pragma endregion

  // overlay
  rr->DrawSprite(RenderItem{
    overlay_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)},
    },
    {{0, 0}, {8, 8}},
    color::setOpacity(fade_overlay_color_, fade_overlay_alpha_current_)
  });
}
