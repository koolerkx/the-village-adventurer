module;

module game.ui.status_ui;

import game.ui.interpolation;

StatusUI::StatusUI(GameContext* ctx) {
  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  auto& tm = ctx->render_resource_manager->texture_manager;

  background_texture_id_ = tm->Load(L"assets/block_white.png");
  ui_texture_id_ = tm->Load(L"assets/ui.png");

  std::vector<InputHint> input_hints = {
    InputHint{L"確認", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
  };
  std::vector<InputHint> x_button_input_hints = {
    InputHint{L"確認", {XButtonCode::A}},
  };

  input_hint_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                       1, input_hints, false, false
                                                     });
  x_button_input_hints_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                 1, x_button_input_hints, false, false
                                                               });
}

void StatusUI::OnUpdate(GameContext* ctx, float delta_time) {
  movement_acc_ += delta_time;

  opacity_ = interpolation::UpdateSmoothValue(
    opacity_,
    1.0f,
    delta_time,
    interpolation::SmoothType::EaseOut,
    3.0f
  );

  if (1.0f - opacity_ <= 0.05 && fade_end_cb_) {
    fade_end_cb_();
  }

  input_hint_->SetOpacity(opacity_);
  x_button_input_hints_->SetOpacity(opacity_);

  input_hint_->OnUpdate(ctx, delta_time);
  x_button_input_hints_->OnUpdate(ctx, delta_time);
}

void StatusUI::OnFixedUpdate(GameContext* ctx, float delta_time) {
  input_hint_->OnFixedUpdate(ctx, delta_time);
  x_button_input_hints_->OnFixedUpdate(ctx, delta_time);
}

void StatusUI::OnRender(GameContext* ctx, Camera*) {
  auto& rr = ctx->render_resource_manager->renderer;
  std::wstringstream wss;

  std::vector<RenderInstanceItem> ui_render_instances = {};

  const float win_center_x = ctx->window_width / 2.0f;
  const float win_center_y = ctx->window_height / 2.0f;

  constexpr float frame_width = 840;
  constexpr float frame_height = 480;
  constexpr float frame_padding = 10;
  constexpr float frame_content_width = frame_width - frame_padding * 2;
  constexpr float frame_content_height = frame_height - frame_padding * 2;
  const float frame_content_x = win_center_x - frame_content_width / 2;
  const float frame_content_y = win_center_y - frame_content_height / 2;

#pragma region Background Overlay
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

#pragma region Background Frame
  ui_render_instances.emplace_back(RenderInstanceItem{
    .transform = {
      .position = {-frame_width / 2, -frame_height / 2, 0},
      .size = {frame_width, frame_height},
      .position_anchor = {win_center_x, win_center_y, 0}
    },
    .uv = {{background_uv_pos_.x, background_uv_pos_.y}, {background_uv_size_.x, background_uv_size_.y}},
    .color = color::setOpacity(color::white, opacity_)
  });
#pragma endregion

  rr->DrawSpritesInstanced(ui_render_instances, ui_texture_id_, {}, true);
  ui_render_instances.clear();

  rr->SetScissorRect(frame_content_x, frame_content_y,
                     frame_content_x + frame_content_width, frame_content_y + frame_content_height);

#pragma region Status Bar
  // Status Bar Frame
  constexpr float margin_top = 52;
  constexpr float margin_left = 142;
  constexpr float status_bar_frame_width = 320;
  constexpr float status_bar_frame_height = 64;

  const float status_bar_frame_center_x = frame_content_x + margin_left + status_bar_frame_width / 2;
  const float status_bar_frame_center_y = frame_content_y + margin_top + status_bar_frame_height / 2;

  const float status_bar_frame_x = status_bar_frame_center_x - status_bar_frame_width / 2;
  const float status_bar_frame_y = status_bar_frame_center_y - status_bar_frame_height / 2;

  ui_render_instances.emplace_back(RenderInstanceItem{
    .transform = {
      .position = {status_bar_frame_x, status_bar_frame_y, 0},
      .size = {status_bar_frame_width, status_bar_frame_height},
    },
    .uv = {
      {status_bar_frame_uv_pos_.x, status_bar_frame_uv_pos_.y},
      {status_bar_frame_uv_size_.x, status_bar_frame_uv_size_.y}
    },
    .color = color::setOpacity(color::white, opacity_)
  });

  // Circle
  constexpr float circle_radius = 36;
  constexpr float circle_margin_top = -4;
  constexpr float circle_margin_left = 4;

  const float circle_x = status_bar_frame_x + circle_margin_left;
  const float circle_y = status_bar_frame_y + circle_margin_top;
  const float circle_center_x = circle_x + circle_radius;
  // const float circle_center_y = circle_y + circle_radius;

  ui_render_instances.emplace_back(RenderInstanceItem{
    .transform = {
      .position = {circle_x, circle_y, 0},
      .size = {circle_radius * 2, circle_radius * 2}
    },
    .uv = {{circle_uv_pos_.x, circle_uv_pos_.y}, {circle_uv_size_.x, circle_uv_size_.y}},
    .color = color::setOpacity(color::white, opacity_)
  });

  // HP Bar Frame
  constexpr float hp_bar_frame_width = 200;
  constexpr float hp_bar_frame_height = 24;
  constexpr float hp_bar_frame_margin_left = 86;

  const float hp_bar_frame_x = status_bar_frame_x + hp_bar_frame_margin_left;
  const float hp_bar_frame_y = status_bar_frame_center_y - hp_bar_frame_height / 2;

  // HP Bar
  constexpr float hp_bar_width = 198;
  constexpr float hp_bar_height = 22;
  constexpr float hp_bar_margin_top = 1;
  constexpr float hp_bar_margin_left = 1;

  const float hp_bar_x = hp_bar_frame_x + hp_bar_margin_left;
  const float hp_bar_y = hp_bar_frame_y + hp_bar_margin_top;

  float hp_bar_fill_width = hp_bar_width * (props_.hp / props_.max_hp);

  // HP Bar
  ui_render_instances.emplace_back(RenderInstanceItem{
    .transform = {
      .position = {hp_bar_x, hp_bar_y, 0},
      .size = {hp_bar_fill_width, hp_bar_height}
    },
    .uv = {{hp_bar_uv_pos_.x, hp_bar_uv_pos_.y}, {hp_bar_uv_size_.x, hp_bar_uv_size_.y}},
    .color = color::setOpacity(color::white, opacity_)
  });

  // HP Bar Frame
  ui_render_instances.emplace_back(RenderInstanceItem{
    .transform = {
      .position = {hp_bar_frame_x, hp_bar_frame_y, 0},
      .size = {hp_bar_frame_width, hp_bar_frame_height}
    },
    .uv = {{hp_bar_frame_uv_pos_.x, hp_bar_frame_uv_pos_.y}, {hp_bar_frame_uv_size_.x, hp_bar_frame_uv_size_.y}},
    .color = color::setOpacity(color::white, opacity_)
  });
#pragma endregion

  rr->DrawSpritesInstanced(ui_render_instances, ui_texture_id_, {}, true);
  ui_render_instances.clear();


#pragma region Text
#pragma region Title text
  constexpr float TITLE_TEXT_MARGIN_TOP = 10;
  const float title_text_center_x = win_center_x;
  const float title_text_center_y = frame_content_y + TITLE_TEXT_MARGIN_TOP;

  wss.str(L"");
  wss << L"ステータス";

  const StringSpriteProps title_props = {
    .pixel_size = 32.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  auto title_size = default_font_->GetStringSize(wss.str(), {}, title_props);

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {title_text_center_x - title_size.width / 2, title_text_center_y, 0}
               }, title_props, {});
#pragma endregion

#pragma region Level text
  // Level Text  
  constexpr float level_text_margin_top = 17;
  const float level_text_center_x = circle_center_x;
  const float level_text_y = circle_y + level_text_margin_top;
  wss.str(L"");
  wss << L"レベル";

  const StringSpriteProps level_title_props = {
    .pixel_size = 16.0f,
    .color = color::setOpacity(color::white, opacity_)
  };
  auto level_title_size = default_font_->GetStringSize(wss.str(), {}, level_title_props);

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {level_text_center_x - level_title_size.width / 2, level_text_y, 0}
               }, level_title_props, {});

  // Level Value
  constexpr float level_value_margin_top = 30;
  const float level_value_y = circle_y + level_value_margin_top;

  wss.str(L"");
  wss << props_.level;
  const StringSpriteProps level_value_props = {
    .pixel_size = 32.0f,
    .color = color::setOpacity(color::white, opacity_)
  };
  auto level_value_size = default_font_->GetStringSize(wss.str(), {}, level_value_props);

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {level_text_center_x - level_value_size.width / 2, level_value_y, 0}
               }, level_value_props, {});
#pragma endregion

#pragma region Game State text
  constexpr float game_state_text_margin_left = 16;

  wss.str(L"");
  wss << L"経過時間：";
  wss << std::setw(2) << std::setfill(L'0') << timer_elapsed_minute_;
  wss << ":";
  wss << std::setw(2) << std::setfill(L'0') << timer_elapsed_seconds_;
  wss << L"\n";
  wss << L"撃退した魔物：";
  wss << props_.monster_killed;

  const StringSpriteProps game_state_text_props = {
    .pixel_size = 20.0f,
    .line_spacing = 10.0f,
    .color = color::setOpacity(color::white, opacity_)
  };
  auto game_state_text_size = default_font_->GetStringSize(wss.str(), {}, game_state_text_props);

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {
                   status_bar_frame_x + status_bar_frame_width + game_state_text_margin_left,
                   status_bar_frame_center_y - game_state_text_size.height / 2,
                   0
                 }
               }, game_state_text_props, {});


#pragma endregion

  const StringSpriteProps section_title_props = {
    .pixel_size = 24.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

#pragma region Ability Text
  constexpr float ability_text_margin_top = 132;
  constexpr float ability_text_margin_left = 38;

  const float ability_text_x = frame_content_x + ability_text_margin_left;
  const float ability_text_y = frame_content_y + ability_text_margin_top;

  wss.str(L"");
  wss << L"能力値";

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {ability_text_x, ability_text_y, 0}
               }, section_title_props, {});

  constexpr float ability_item_margin_top = 22;
  const float ability_item_y = ability_text_y + section_title_props.pixel_size + ability_item_margin_top;

  const StringSpriteProps ability_item_props = {
    .pixel_size = 20.0f,
    .line_spacing = 10.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  wss.str(L"");

  // HP
  wss << L"HP　：";
  wss << static_cast<int>(props_.hp);
  wss << L" / ";
  wss << static_cast<int>(props_.max_hp);
  wss << L"\n";

  // Defense
  wss << L"防御力　：";
  wss << static_cast<int>(props_.defense);
  // wss << L" + ";
  // wss << L"20";
  wss << L"\n";

  // Attack
  wss << L"攻撃力　：";
  wss << static_cast<int>(props_.attack);
  // wss << L" + ";
  // wss << L"20";
  wss << L"\n";

  // Moving
  wss << L"移動速度：";
  wss << static_cast<int>(props_.speed);
  // wss << L" + ";
  // wss << L"20";
  wss << L"\n";

  // Exp
  wss << L"経験値　：";
  wss << props_.experience;
  wss << L" / ";
  wss << props_.max_experience;
  wss << L"\n";

  // Total Exp
  wss << L"総経験値：";
  wss << props_.total_experience;

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {ability_text_x, ability_item_y, 0}
               }, ability_item_props, {});

#pragma endregion
#pragma region Level Up Ability Text
  constexpr float level_up_ability_text_margin_top = 132;
  constexpr float level_up_ability_text_margin_left = 280;

  const float level_up_ability_text_x = frame_content_x + level_up_ability_text_margin_left;
  const float level_up_ability_text_y = frame_content_y + level_up_ability_text_margin_top;

  wss.str(L"");
  wss << L"成長ボーナス";

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {level_up_ability_text_x, level_up_ability_text_y, 0}
               }, section_title_props, {});

  constexpr float level_up_ability_item_margin_top = 22;
  constexpr float level_up_ability_item_margin_left = 48;
  const float level_up_ability_item_x = level_up_ability_text_x + level_up_ability_item_margin_left;
  const float level_up_ability_item_y =
    level_up_ability_text_y + section_title_props.pixel_size + level_up_ability_item_margin_top;

  const StringSpriteProps ability_item_text_props = {
    .pixel_size = 16.0f,
    .line_height = 36.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  wss.str(L"");
  for (int i = 0; i < props_.abilities.size(); i++) {
    auto& a = props_.abilities[i];
    wss << player_level::GetAbilityDescription(a.type);
    if (a.type == player_level::Ability::HP_UP) {
      wss << " ";
      wss << std::fixed << std::setprecision(2);
      wss << a.value;
      wss << " ";
    }
    else {
      wss << " ";
      wss << std::fixed << std::setprecision(2);
      wss << a.multiplier;
      wss << L" 倍";
    }
    wss << L"\n";

    constexpr float ability_icon_size = 36;
    constexpr float ability_icon_margin_top = 12;

    const float ability_icon_start_x = level_up_ability_text_x;
    const float ability_icon_start_y = level_up_ability_text_y + section_title_props.pixel_size;
    constexpr float ability_item_gap = 12;

    ui_render_instances.emplace_back(RenderInstanceItem{
      .transform = {
        .position = {
          ability_icon_start_x,
          ability_icon_start_y + ability_icon_margin_top + (ability_item_gap + ability_icon_size) * i, 0
        },
        .size = {36, 36}
      },
      .uv = player_level::GetAbilityUV(a.type),
      .color = color::setOpacity(color::white, opacity_)
    });
  }

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {level_up_ability_item_x, level_up_ability_item_y, 0}
               }, ability_item_text_props, {});
#pragma endregion
#pragma region Buff Text
  constexpr float buff_text_margin_top = 132;
  constexpr float buff_text_margin_left = 522;

  const float buff_text_x = frame_content_x + buff_text_margin_left;
  const float buff_text_y = frame_content_y + buff_text_margin_top;

  wss.str(L"");
  wss << L"バフ";

  rr->DrawFont(wss.str(), font_key_, {
                 .position = {buff_text_x, buff_text_y, 0}
               }, section_title_props, {});

  constexpr float buff_item_margin_top = 22;
  constexpr float buff_margin_left = 48;
  const float buff_item_x = buff_text_x + buff_margin_left;
  const float buff_item_y =
    buff_text_y + section_title_props.pixel_size + buff_item_margin_top;

  const StringSpriteProps buff_text_props = {
    .pixel_size = 16.0f,
    .line_height = 36.0f,
    .color = color::setOpacity(color::white, opacity_)
  };

  wss.str(L"");
  if (props_.buffs.size() == 0) {
    wss << L"なし";

    rr->DrawFont(wss.str(), font_key_, {
                   .position = {buff_text_x, buff_item_y, 0}
                 }, buff_text_props, {});
  }
  else {
    for (int i = 0; i < props_.buffs.size(); i++) {
      auto& a = props_.buffs[i];
      wss << GetBuffDisplayText(a.type);
      if (a.type != BuffType::INVINCIBLE) {
        wss << L"：残り";
        wss << std::fixed << std::setprecision(0);
        wss << a.duration - a.elapsed;
        wss << L"秒";
      }
      wss << L"\n";

      constexpr float ability_icon_size = 36;
      constexpr float ability_icon_margin_top = 12;

      const float ability_icon_start_x = buff_text_x;
      const float ability_icon_start_y = buff_text_y + section_title_props.pixel_size;
      constexpr float ability_item_gap = 12;

      ui_render_instances.emplace_back(RenderInstanceItem{
        .transform = {
          .position = {
            ability_icon_start_x,
            ability_icon_start_y + ability_icon_margin_top + (ability_item_gap + ability_icon_size) * i, 0
          },
          .size = {36, 36}
        },
        .uv = GetBuffIconUV(a.type),
        .color = color::setOpacity(color::white, opacity_)
      });
    }

    rr->DrawFont(wss.str(), font_key_, {
                   .position = {buff_item_x, buff_item_y, 0}
                 }, buff_text_props, {});
  }
#pragma endregion

  // For ability icon
  rr->DrawSpritesInstanced(ui_render_instances, ui_texture_id_, {}, true);
#pragma endregion

  rr->ResetScissorRect();

#pragma region back button
  constexpr float button_width = 200;
  constexpr float button_height = 50;
  constexpr float button_y_offset = 20;
  constexpr float button_margin_top = 16;

  float back_button_center_x = ctx->window_width / 2.0f;
  float back_button_center_y = frame_content_y + frame_content_height + frame_padding
    + button_margin_top + button_y_offset;
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
  float selected_frame_x = back_button_center_x;
  float selected_frame_y = back_button_center_y;

  float selected_width = button_width + selection_frame_padding + selected_frame_moving_range_ * std::abs(
    std::cos(selected_frame_moving_speed_ * movement_acc_));
  float selected_height = button_height + selection_frame_padding + selected_frame_moving_range_ * std::abs(
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
#pragma endregion

  if (is_x_input_) {
    x_button_input_hints_->OnRender(ctx);
  }
  else {
    input_hint_->OnRender(ctx);
  }
}
