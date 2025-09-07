module;

module game.ui.result_ui;

import std;
import game.ui.interpolation;

ResultUI::ResultUI(GameContext* ctx) {
  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  auto& tm = ctx->render_resource_manager->texture_manager;

  background_1_texture_id_ = tm->Load(L"assets/result_bg_1.png");
  background_2_texture_id_ = tm->Load(L"assets/result_bg_2.png");
  background_3_texture_id_ = tm->Load(L"assets/result_bg_3.png");
  background_4_texture_id_ = tm->Load(L"assets/result_bg_4.png");
  ui_texture_id_ = tm->Load(L"assets/ui.png");
  overlay_texture_id_ = tm->Load(L"assets/block_white.png");
  
  std::vector<InputHint> input_hints = {
    InputHint{L"確認", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
    InputHint{L"選択", {KeyCode::KK_W, KeyCode::KK_S, KeyCode::KK_UP, KK_DOWN}}
  };
  std::vector<InputHint> x_button_input_hints = {
    InputHint{L"確認", {XButtonCode::A}},
    InputHint{L"選択", {XButtonCode::LeftThumb, XButtonCode::DPadUp, XButtonCode::DPadDown}}
  };
  
  input_hint_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                       1, input_hints, false, true
                                                     });
  x_button_input_hints_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                 1, x_button_input_hints, false, true
                                                               });

}

void ResultUI::OnUpdate(GameContext* ctx, float delta_time) {
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

  x_button_input_hints_->OnUpdate(ctx, delta_time);
  input_hint_->OnUpdate(ctx, delta_time);
}

void ResultUI::OnFixedUpdate(GameContext* ctx, float delta_time) {
  x_button_input_hints_->OnFixedUpdate(ctx, delta_time);
  input_hint_->OnFixedUpdate(ctx, delta_time);
}

void ResultUI::OnRender(GameContext* ctx, Camera*) {
  auto& rr = ctx->render_resource_manager->renderer;

  rr->DrawSprite(RenderItem{
    background_1_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_, 0}, {576, 324}},
    color::grey700
  });

  rr->DrawSprite(RenderItem{
    background_2_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_ * 0.6f, 0}, {576, 324}},
    color::grey700
  });

  rr->DrawSprite(RenderItem{
    background_3_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_ * 0.8f, 0}, {576, 324}},
    color::grey700
  });

  rr->DrawSprite(RenderItem{
    background_4_texture_id_,
    Transform{
      {0, 0, 0},
      {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)}
    },
    {{uv_horizontal_offset_ * 1.2f, 0}, {576, 324}},
    color::grey700
  });

  // Game Over Title
  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        static_cast<float>(ctx->window_width) / 2 - 638 / 2,
        85,
        0
      },
      {638, 191}
    },
    {
      {title_background_uv_pos_.x, title_background_uv_pos_.y},
      {title_background_uv_size_.x, title_background_uv_size_.y}
    },
    color::white
  });
  auto title_text_props = StringSpriteProps{
    .pixel_size = 64.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::white
  };

  auto title_text_size = default_font_->GetStringSize(L"GAME OVER", {}, title_text_props);

  rr->DrawFont(L"GAME OVER",
               font_key_,
               Transform{
                 .position = {
                   static_cast<float>(ctx->window_width) / 2 - title_text_size.width / 2,
                   128,
                   0
                 }
               },
               title_text_props);

  // Result
  std::wstringstream wss;
  auto make_left = [&](const std::wstring& label, const std::wstring& expr) {
    std::wstringstream tmp;
    tmp << label << expr;
    return tmp.str();
  };
  auto output_line = [&](const std::wstring& left, const std::wstring& right) {
    wss << std::left << std::setw(18) << left
      << L"= " << right << L"\n";
  };
  {
    std::wstringstream expr;
    expr << monster_killed_ << L" × " << multiplier_monster_;
    output_line(make_left(L"撃退した魔物：", expr.str()),
                std::to_wstring(monster_killed_ * multiplier_monster_));
  }
  {
    std::wstringstream expr;
    expr << std::setw(2) << std::setfill(L'0') << minutes_
      << L":" << std::setw(2) << std::setfill(L'0') << seconds_;
    output_line(make_left(L"冒険した時間：", expr.str()),
                std::to_wstring(static_cast<int>(minutes_ * 60 + seconds_) * multiplier_time_));
  }
  {
    std::wstringstream expr;
    expr << level_ << L" × " << multiplier_level_;
    output_line(make_left(L"最終レベル　：", expr.str()),
                std::to_wstring(level_ * multiplier_level_));
  }
  {
    std::wstringstream expr;
    expr << score_;
    output_line(make_left(L"スコア　　　：", expr.str()),
                std::to_wstring(score_));
  }

  auto stat_text_prop = StringSpriteProps{
    .pixel_size = 32.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .line_spacing = 10.0f,
    .color = color::white
  };

  auto stat_text = default_font_->GetStringSize(wss.str(), {}, stat_text_prop);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   static_cast<float>(ctx->window_width) / 2 - stat_text.width / 2,
                   static_cast<float>(ctx->window_height) / 2 - stat_text.height / 2,
                   0
                 }
               },
               stat_text_prop);

  // Start Button
  float start_button_center_x = static_cast<float>(ctx->window_width) / 2;
  float start_button_center_y = static_cast<float>(ctx->window_height) / 2 + 140;

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
    color::white
  });

  auto start_text_props = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::white
  };

  auto start_text_size = default_font_->GetStringSize(L"復活", {}, start_text_props);

  rr->DrawFont(L"復活",
               font_key_,
               Transform{
                 .position = {
                   start_button_center_x - start_text_size.width / 2,
                   start_button_center_y - start_text_size.height / 2,
                   0
                 }
               },
               start_text_props);

  // End Button
  float end_button_center_x = static_cast<float>(ctx->window_width) / 2;
  float end_button_center_y = static_cast<float>(ctx->window_height) / 2 + 220;

  auto end_text_props = StringSpriteProps{
    .pixel_size = 20.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::white
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
    color::white
  });

  auto end_text_size = default_font_->GetStringSize(L"タイトル画面に戻る", {}, end_text_props);

  rr->DrawFont(L"タイトル画面に戻る",
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
  float selected_frame_x = selected_option_ == 0 ? start_button_center_x : end_button_center_x;
  float selected_frame_y = selected_option_ == 0 ? start_button_center_y : end_button_center_y;

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
    color::white
  });

  // Version
  std::wstring version_text = L"v0.2";
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

  // Ranking
  wss.str(L"");

  wss << L"冒険者ランキング";
  for (int i = 0; i < 5; i++) {
    if (i < ranking_.size()) {
      auto ranking_item = ranking_[i];
      wss << "\n" << EpochToDateTime(ranking_item.timestamp_ms) << " ";
      wss << ranking_item.score;
    }
    else {
      wss << "\n";
    }
  }

  auto ranking_text_prop = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 26.0f,
    .color = color::white
  };
  auto ranking_text = default_font_->GetStringSize(wss.str(), {}, ranking_text_prop);
  constexpr float ranking_text_margin = 16.0f;

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   // static_cast<float>(ctx->window_width) - ranking_text.width - ranking_text_margin,
                   ranking_text_margin,
                   static_cast<float>(ctx->window_height) - ranking_text.height - ranking_text_margin -
                   version_text_size.height - version_text_padding,
                   0
                 }
               },
               ranking_text_prop);

  if (is_x_input_) {
    x_button_input_hints_->OnRender(ctx);
  }
  else {
    input_hint_->OnRender(ctx);
  }

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
