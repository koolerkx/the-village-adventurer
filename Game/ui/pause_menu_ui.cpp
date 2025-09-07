module;

module game.ui.pause_ui;

PauseMenuUI::PauseMenuUI(GameContext* ctx) {
  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  auto& tm = ctx->render_resource_manager->texture_manager;

  title_texture_id_ = tm->Load(L"assets/title.png");
  background_texture_id_ = tm->Load(L"assets/block_white.png");
  ui_texture_id_ = tm->Load(L"assets/ui.png");
  
  std::vector<InputHint> input_hints = {
    InputHint{L"確認", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
    InputHint{L"選択", {KeyCode::KK_W, KeyCode::KK_S, KeyCode::KK_UP, KK_DOWN}}
  };
  std::vector<InputHint> x_button_input_hints = {
    InputHint{L"確認", {XButtonCode::A}},
    InputHint{L"選択", {XButtonCode::LeftThumb, XButtonCode::DPadUp, XButtonCode::DPadDown}},
  };

  input_hint_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                       1, input_hints, false, false
                                                     });
  x_button_input_hints_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                 1, x_button_input_hints, false, false
                                                               });
}

void PauseMenuUI::OnUpdate(GameContext* ctx, float delta_time) {
  movement_acc_ += delta_time;

  input_hint_->OnUpdate(ctx, delta_time);
  x_button_input_hints_->OnUpdate(ctx, delta_time);
}

void PauseMenuUI::OnFixedUpdate(GameContext* ctx, float delta_time) {
  input_hint_->OnFixedUpdate(ctx, delta_time);
  x_button_input_hints_->OnFixedUpdate(ctx, delta_time);
}

void PauseMenuUI::OnRender(GameContext* ctx, Camera*) {
  auto& rr = ctx->render_resource_manager->renderer;
  std::wstringstream wss;

  Vector2 win_size = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)};

  rr->DrawSprite(background_texture_id_,
                 Transform{
                   {0, 0, 0},
                   {win_size.x, win_size.y}
                 },
                 UV{{0, 0}, {8, 8}},
                 color::setOpacity(color::black, 0.7f)
  );

  // Pause menu title text
  wss.str(L"");
  wss << L"PAUSE　一時停止";
  auto title_text_props = StringSpriteProps{
    .pixel_size = 48.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::white
  };
  auto title_text_size = default_font_->GetStringSize(wss.str(), {}, title_text_props);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   win_size.x / 2 - title_text_size.width / 2,
                   win_size.y / 2 - 72 - 48,
                   0
                 }
               }, title_text_props
  );

  // Button Frame
  constexpr float button_width = 200;
  constexpr float button_height = 50;
  constexpr float button_y_offset = 20;

  float back_button_center_x = win_size.x / 2;
  float back_button_center_y = win_size.y / 2 + button_y_offset;
  wss.str(L"");
  wss << L"冒険に戻る";

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
    color::white
  });

  auto back_text_props = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::white
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

  float end_button_center_x = win_size.x / 2;
  float end_button_center_y = win_size.y / 2 + button_height + 20 + button_y_offset;
  wss.str(L"");
  wss << L"冒険をやめる";

  auto end_text_props = StringSpriteProps{
    .pixel_size = 24.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::white
  };

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        end_button_center_x - button_width / 2,
        end_button_center_y - button_height / 2,
        0
      },
      {button_width, button_height}
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::white
  });

  auto end_text_size = default_font_->GetStringSize(wss.str(), {}, end_text_props);

  rr->DrawFont(wss.str(),
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
  constexpr float frame_padding = 4.0f;
  float selected_frame_x = selected_option_ == 0 ? back_button_center_x : end_button_center_x;
  float selected_frame_y = selected_option_ == 0 ? back_button_center_y : end_button_center_y;

  float selected_width = button_width + frame_padding + selected_frame_moving_range_ * std::abs(
    std::cos(selected_frame_moving_speed_ * movement_acc_));
  float selected_height = button_height + frame_padding + selected_frame_moving_range_ * std::abs(
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
  
  if (is_x_input_) {
    x_button_input_hints_->OnRender(ctx);
  }
  else {
    input_hint_->OnRender(ctx);
  }
}
