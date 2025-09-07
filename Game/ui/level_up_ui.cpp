module;

module game.ui.level_up;
import game.ui.interpolation;

LevelUpUI::LevelUpUI(GameContext* ctx) {
  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  auto& tm = ctx->render_resource_manager->texture_manager;

  background_texture_id_ = tm->Load(L"assets/block_white.png");
  kamifubuki_texture_id_ = tm->Load(L"assets/kamifubuki64.png");
  ui_texture_id_ = tm->Load(L"assets/ui.png");

  std::vector<InputHint> input_hints = {
    InputHint{L"確認", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
    InputHint{L"選択", {KeyCode::KK_A, KeyCode::KK_D, KeyCode::KK_LEFT, KeyCode::KK_RIGHT}}
  };
  std::vector<InputHint> x_button_input_hints = {
    InputHint{L"確認", {XButtonCode::A}},
    InputHint{L"選択", {XButtonCode::LeftThumb, XButtonCode::DPadLeft, XButtonCode::DPadRight}},
  };

  input_hint_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                       1, input_hints, false, false
                                                     });
  x_button_input_hints_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                 1, x_button_input_hints, false, false
                                                               });
}

void LevelUpUI::OnUpdate(GameContext* ctx, float delta_time) {
  movement_acc_ += delta_time;

  kamifubuki_frame_timeout -= delta_time;
  if (kamifubuki_frame_timeout <= 0) {
    current_kamifubuki_ = (current_kamifubuki_ + 1) % frame;
    kamifubuki_frame_timeout += timeout_per_frame;
  }

  opacity_current_ = interpolation::UpdateSmoothValue(
    opacity_current_,
    opacity_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    1.0f
  );

  if (opacity_target_ - opacity_current_ <= 0.95) {
    fade_in_callback_();
  }

  input_hint_->OnUpdate(ctx, delta_time);
  x_button_input_hints_->OnUpdate(ctx, delta_time);
}

void LevelUpUI::OnFixedUpdate(GameContext* ctx, float delta_time) {
  input_hint_->OnFixedUpdate(ctx, delta_time);
  x_button_input_hints_->OnFixedUpdate(ctx, delta_time);
}

void LevelUpUI::OnRender(GameContext* ctx, Camera*) {
  auto& rr = ctx->render_resource_manager->renderer;
  std::wstringstream wss;

  Vector2 win_size = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)};

  rr->DrawSprite(background_texture_id_,
                 Transform{
                   {0, 0, 0},
                   {win_size.x, win_size.y}
                 },
                 UV{{0, 0}, {8, 8}},
                 color::setOpacity(color::black, 0.7f * opacity_current_)
  );

  // kamifubuki
  UV kamifubuki_uv = {
    {
      start_uv.position.x + start_uv.size.x * (current_kamifubuki_ % frame_per_line),
      start_uv.position.y + start_uv.size.y * (current_kamifubuki_ / frame_per_line),
    },
    start_uv.size
  };

  rr->DrawSprite(kamifubuki_texture_id_,
                 Transform{
                   {0, 0, 0},
                   {win_size.x, win_size.y}
                 },
                 kamifubuki_uv,
                 color::setOpacity(color::grey500, 0.6f * opacity_current_)
  );

  // Level menu title text
  wss.str(L"");
  wss << L"レベルアップ";
  auto title_text_props = StringSpriteProps{
    .pixel_size = 48.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_current_)
  };
  auto title_text_size = default_font_->GetStringSize(wss.str(), {}, title_text_props);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   win_size.x / 2 - title_text_size.width / 2,
                   win_size.y / 2 - 184 - 48,
                   0
                 }
               }, title_text_props
  );

  wss.str(L"");
  wss << L"報酬を選んでください";
  auto subtitle_text_props = StringSpriteProps{
    .pixel_size = 36.0,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_current_)
  };
  auto subtitle_text_size = default_font_->GetStringSize(wss.str(), {}, subtitle_text_props);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   win_size.x / 2 - subtitle_text_size.width / 2,
                   win_size.y / 2 - 132 - 48,
                   0
                 }
               }, subtitle_text_props
  );

#pragma region Button Frame
  constexpr float button_width = 200;
  constexpr float button_height = 50;
  constexpr float button_y_offset = 147 + button_height / 2;

  wss.str(L"");
  wss << L"この報酬を選択";
  auto select_text_props = StringSpriteProps{
    .pixel_size = 20.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_current_)
  };
  auto select_text_size = default_font_->GetStringSize(wss.str(), {}, select_text_props);

  constexpr float option_1_x_offset = -(150 + button_width / 2);
  float option1_x = win_size.x / 2 + option_1_x_offset;
  float option1_y = win_size.y / 2 + button_y_offset;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option1_x - button_width / 2,
        option1_y - button_height / 2,
        0
      },
      {button_width, button_height},
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   option1_x - select_text_size.width / 2,
                   option1_y - select_text_size.height / 2,
                   0
                 }
               },
               select_text_props);

  constexpr float option_2_x_offset = 0;
  float option2_x = win_size.x / 2 + option_2_x_offset;
  float option2_y = win_size.y / 2 + button_y_offset;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option2_x - button_width / 2,
        option2_y - button_height / 2,
        0
      },
      {button_width, button_height},
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   option2_x - select_text_size.width / 2,
                   option2_y - select_text_size.height / 2,
                   0
                 }
               },
               select_text_props);

  constexpr float option_3_x_offset = (150 + button_width / 2);
  float option3_x = win_size.x / 2 + option_3_x_offset;
  float option3_y = win_size.y / 2 + button_y_offset;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option3_x - button_width / 2,
        option3_y - button_height / 2,
        0
      },
      {button_width, button_height},
    },
    {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   option3_x - select_text_size.width / 2,
                   option3_y - select_text_size.height / 2,
                   0
                 }
               },
               select_text_props);

  // Selected Frame
  constexpr float frame_padding = 4.0f;
  float selected_frame_x = selected_option_ == 0 ? option1_x : selected_option_ == 1 ? option2_x : option3_x;
  float selected_frame_y = selected_option_ == 0 ? option1_y : selected_option_ == 1 ? option2_y : option3_y;

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
    color::setOpacity(color::white, opacity_current_)
  });
#pragma endregion

#pragma region options box
  float box_width = 206;
  float box_height = 206;
  float box_y_offset = 0;
  float box_y = win_size.y / 2 + box_y_offset;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option1_x - box_width / 2,
        box_y - box_height / 2,
        0
      },
      {box_width, box_height},
    },
    {
      {option_frame_uv_pos_.x, option_frame_uv_pos_.y},
      {option_frame_uv_size_.x, option_frame_uv_size_.y}
    },
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option2_x - box_width / 2,
        box_y - box_height / 2,
        0
      },
      {box_width, box_height},
    },
    {
      {option_frame_uv_pos_.x, option_frame_uv_pos_.y},
      {option_frame_uv_size_.x, option_frame_uv_size_.y}
    },
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option3_x - box_width / 2,
        box_y - box_height / 2,
        0
      },
      {box_width, box_height},
    },
    {
      {option_frame_uv_pos_.x, option_frame_uv_pos_.y},
      {option_frame_uv_size_.x, option_frame_uv_size_.y}
    },
    color::setOpacity(color::white, opacity_current_)
  });

  // ICON
  constexpr float icon_width = 128;
  constexpr float icon_height = 128;
  constexpr float icon_y_offset = -15;

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option1_x - icon_width / 2,
        box_y + icon_y_offset - icon_height / 2,
        0
      },
      {icon_width, icon_height},
    },
    options_[0].uv,
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option2_x - icon_width / 2,
        box_y + icon_y_offset - icon_height / 2,
        0
      },
      {icon_width, icon_height},
    },
    options_[1].uv,
    color::setOpacity(color::white, opacity_current_)
  });

  rr->DrawSprite(RenderItem{
    ui_texture_id_,
    Transform{
      {
        option3_x - icon_width / 2,
        box_y + icon_y_offset - icon_height / 2,
        0
      },
      {icon_width, icon_height},
    },
    options_[2].uv,
    color::setOpacity(color::white, opacity_current_)
  });

  // Description text
  auto option_description_text_props = StringSpriteProps{
    .pixel_size = 24.0,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, opacity_current_)
  };
  float description_y_offset = 61;

  wss.str(L"");
  wss << options_[0].text;
  auto option1_text_size = default_font_->GetStringSize(wss.str(), {}, option_description_text_props);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   option1_x - option1_text_size.width / 2,
                   box_y + description_y_offset - option1_text_size.height / 2,
                   0
                 }
               }, option_description_text_props
  );

  wss.str(L"");
  wss << options_[1].text;
  auto option2_text_size = default_font_->GetStringSize(wss.str(), {}, option_description_text_props);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   option2_x - option2_text_size.width / 2,
                   box_y + description_y_offset - option2_text_size.height / 2,
                   0
                 }
               }, option_description_text_props
  );


  wss.str(L"");
  wss << options_[2].text;
  auto option3_text_size = default_font_->GetStringSize(wss.str(), {}, option_description_text_props);

  rr->DrawFont(wss.str(),
               font_key_,
               Transform{
                 .position = {
                   option3_x - option3_text_size.width / 2,
                   box_y + description_y_offset - option3_text_size.height / 2,
                   0
                 }
               }, option_description_text_props
  );


#pragma endregion
  
  if (is_x_input_) {
    x_button_input_hints_->OnRender(ctx);
  }
  else {
    input_hint_->OnRender(ctx);
  }
}
