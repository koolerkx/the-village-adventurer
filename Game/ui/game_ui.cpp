module;

module game.ui.game_ui;

import std;
import graphic.utils.types;
import graphic.utils.font;
import game.ui.interpolation;
import game.math;

void GameUI::PlayEnterAreaMessage(std::wstring message) {
  area_message_ = message;
  area_message_opacity_current_ = 1.0f;
  is_showing_area_message_ = true;
}

GameUI::GameUI(GameContext* ctx, SceneContext*, std::wstring texture_path) {
  texture_id_ = ctx->render_resource_manager->texture_manager->Load(texture_path);

  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);

  fade_overlay_texture_id_ = ctx->render_resource_manager->texture_manager->Load(L"assets/block_white.png");

  std::vector<InputHint> input_hints = {
    InputHint{L"移動", {KeyCode::KK_W, KeyCode::KK_S, KeyCode::KK_A, KeyCode::KK_D}},
    InputHint{L"    ", {KeyCode::KK_UP, KeyCode::KK_DOWN, KeyCode::KK_LEFT, KeyCode::KK_RIGHT}},
    InputHint{L"攻撃", {KeyCode::KK_SPACE, KeyCode::KK_ENTER}},
    InputHint{L"スキル選択", {KeyCode::KK_Q, KeyCode::KK_E}},
    InputHint{L"キャラ情報", {KeyCode::KK_I, KeyCode::KK_R}},
    InputHint{L"一時停止", {KeyCode::KK_ESCAPE}},
  };
  std::vector<InputHint> x_button_input_hints = {
    InputHint{L"移動", {XButtonCode::LeftThumb}},
    InputHint{L"攻撃", {XButtonCode::A}},
    InputHint{L"スキル選択", {XButtonCode::LB, XButtonCode::RB, XButtonCode::Y, XButtonCode::B}},
    InputHint{L"キャラ情報", {XButtonCode::X, XButtonCode::Back}},
    InputHint{L"一時停止", {XButtonCode::Start}},
  };

  input_hint_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                       1, input_hints, false, true
                                                     });
  x_button_input_hints_ = std::make_unique<InputHintComponent>(ctx, InputHintProps{
                                                                 1, x_button_input_hints, false, true
                                                               });
}

void GameUI::OnUpdate(GameContext* ctx, SceneContext*, float delta_time, Camera* camera) {
  hp_percentage_current_ = interpolation::UpdateSmoothValue(
    hp_percentage_current_,
    hp_percentage_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    1.0f
  );

  heal_flash_opacity_current_ = interpolation::UpdateSmoothValue(
    heal_flash_opacity_current_,
    heal_flash_opacity_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    0.5f
  );

  damage_flash_opacity_current_ = interpolation::UpdateSmoothValue(
    damage_flash_opacity_current_,
    damage_flash_opacity_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    0.5f
  );

  if (is_showing_area_message_) {
    area_message_opacity_current_ = interpolation::UpdateSmoothValue(
      area_message_opacity_current_,
      area_message_opacity_target_,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.25f
    );
    if (area_message_opacity_current_ < 0.1) is_showing_area_message_ = false;
  }

  experience_bar_percentage_current_ = interpolation::UpdateSmoothValue(
    experience_bar_percentage_current_,
    experience_bar_percentage_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    0.5f
  );

  if (std::abs(fade_overlay_alpha_current_ - fade_overlay_alpha_target_) > 0.01f)
    fade_overlay_alpha_current_ = interpolation::UpdateSmoothValue(
      fade_overlay_alpha_current_,
      fade_overlay_alpha_target_,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.5f
    );

  if (fade_overlay_callback_) {
    float diff = std::fabs(fade_overlay_alpha_target_ - fade_overlay_alpha_current_);
    if (diff <= 0.25f) {
      auto cb = fade_overlay_callback_;
      fade_overlay_callback_ = {};
      cb();
    }
  }

  if (std::fabs(ui_opacity_current_ - ui_opacity_target_) > 0.01f)
    ui_opacity_current_ = interpolation::UpdateSmoothValue(
      ui_opacity_current_,
      ui_opacity_target_,
      delta_time,
      interpolation::SmoothType::EaseOut,
      1.0f
    );

  std::erase_if(experience_stars_,
                [target = EXP_COIN_TARGET_POS, &experience_stars_end = experience_stars_end_](
                const ExperienceStar& exp_star) {
                  if (math::GetDistance({exp_star.position.x, exp_star.position.y}, target) <= 2) {
                    exp_star.callback(exp_star.value);
                    experience_stars_end.emplace_back(StarTrajectoryEndEffect{
                      .position = {target.x, target.y, 0}
                    });

                    return true;
                  }
                  return false;
                });
  for (auto& exp_star : experience_stars_) {
    if (exp_star.floating_timeout > 0) {
      exp_star.floating_timeout -= delta_time;
      if (exp_star.floating_timeout <= 0) {
        // on floating time over
        Vector2 screen_pos = camera->TransformToScreenSpace({exp_star.position.x, exp_star.position.y},
                                                            {
                                                              static_cast<float>(ctx->window_width),
                                                              static_cast<float>(ctx->window_height)
                                                            });
        exp_star.position = {screen_pos.x, screen_pos.y, 0};
      }
    }
    else {
      float new_y = interpolation::UpdateSmoothValue(
        exp_star.position.y,
        EXP_COIN_TARGET_POS.y,
        delta_time,
        interpolation::SmoothType::EaseInOut,
        20.0f
      );
      float new_x = interpolation::UpdateSmoothValue(
        exp_star.position.x,
        EXP_COIN_TARGET_POS.x,
        delta_time,
        interpolation::SmoothType::EaseInOut,
        20.0f
      );

      exp_star.position = {new_x, new_y, 0};
    }
  }

  std::erase_if(experience_stars_trajectory_, [](const StarTrajectory& t) { return t.size <= 1; });

  for (auto& t : experience_stars_trajectory_) {
    t.size = interpolation::UpdateSmoothValue(
      t.size,
      0,
      delta_time,
      interpolation::SmoothType::EaseOut,
      3.0f
    );
  }

  std::erase_if(experience_stars_end_, [](const StarTrajectoryEndEffect& t) {
    return t.target - t.size <= 1.0f;
  });

  for (auto& t : experience_stars_end_) {
    t.size = interpolation::UpdateSmoothValue(
      t.size,
      t.target,
      delta_time,
      interpolation::SmoothType::EaseOut,
      3.0f
    );
    t.opacity = interpolation::UpdateSmoothValue(
      t.opacity,
      0.0f,
      delta_time,
      interpolation::SmoothType::EaseOut,
      1.0f
    );
  }

  input_hint_->OnUpdate(ctx, delta_time);
  x_button_input_hints_->OnUpdate(ctx, delta_time);
}

void GameUI::OnFixedUpdate(GameContext* ctx, SceneContext*, float delta_time) {
  // workaround: fixed flash rate in one tick, TODO: use setTimeout instead
  if (is_get_damage_frame_ && is_hp_flashing_) {
    is_hp_flashing_ = false;
    is_get_damage_frame_ = false;
    heal_flash_opacity_target_ = 0;
    damage_flash_opacity_target_ = 0;
  }

  for (auto& text : damage_texts) {
    text.opacity = interpolation::UpdateSmoothValue(
      text.opacity,
      0,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.75f
    );
    text.position.y -= 0.1f;
  }

  damage_texts.erase(std::remove_if(damage_texts.begin(), damage_texts.end(),
                                    [](DamageTextProps text) { return text.opacity <= 0.05; }),
                     damage_texts.end());


  for (auto& text : event_texts) {
    text.opacity = interpolation::UpdateSmoothValue(
      text.opacity,
      0,
      delta_time,
      interpolation::SmoothType::EaseOut,
      0.25f
    );
    text.position.y -= 0.2f;
  }

  event_texts.erase(std::remove_if(event_texts.begin(), event_texts.end(),
                                   [](EventTextProps text) { return text.opacity <= 0.05; }),
                    event_texts.end());

  for (auto& exp_star : experience_stars_) {
    if (exp_star.floating_timeout <= 0) {
      experience_stars_trajectory_.emplace_back(StarTrajectory{
        .position = exp_star.position,
      });
    }
  }

  input_hint_->OnFixedUpdate(ctx, delta_time);
  x_button_input_hints_->OnFixedUpdate(ctx, delta_time);
}

void GameUI::OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera) {
  if (!is_show_ui_) return;

  auto& rr = ctx->render_resource_manager->renderer;
  std::wstringstream wss;

  std::vector<RenderInstanceItem> render_items;

  // Overlay at back
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {0, 0, 0},
      .size = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)},
    },
    texture_map["DamageOverlay"], color::setOpacity(color::white, damage_flash_opacity_current_)
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {0, 0, 0},
      .size = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)},
    },
    texture_map["HealOverlay"], color::setOpacity(color::white, heal_flash_opacity_current_)
  });

  // Session: Left Upper
  // HP Bar: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {24, 24, 0},
      .size = {320, 64},
    },
    texture_map["HPBarPanel"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  // HP Bar: Heart Background Circle
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {28, 20, 0},
      .size = {72, 72},
    },
    texture_map["BigCircle"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  // HP Bar: Heart
  // render_items.emplace_back(RenderInstanceItem{
  //   Transform{
  //     .position = {38, 30, 0},
  //     .size = {52, 52},
  //   },
  //   texture_map["Heart"],
  //   color::setOpacity(color::white, ui_opacity_current_)
  // });
  // HP Bar: HB Bar
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {110, 59, 0},
      .size = {200 * hp_percentage_current_, 14},
    },
    texture_map["HPBar"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  if (is_get_damage_frame_) {
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {110, 59, 0},
        .size = {200 * hp_percentage_current_, 14},
      },
      texture_map["Block"],
      color::setOpacity(color::white, ui_opacity_current_)
    });
    is_hp_flashing_ = true;
  }
  // HP Bar: HB Bar Frame
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {106, 54, 0},
      .size = {208, 24},
    },
    texture_map["HPBarFrame"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

  // Session: Left Upper
  // Buff list: Background
  constexpr float buff_box_padding = 10.0f;
  constexpr float buff_item_gap = 10.0f;
  int item_count = static_cast<int>(player_buffs_.size());
  if (item_count > 0) {
    float buff_box_height = item_count * 32 + (item_count - 1) * buff_item_gap + buff_box_padding * 2;
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {24, 108, 0},
        .size = {164, buff_box_height},
      },
      texture_map["Block"], color::setOpacity(color::black, 0.25f * ui_opacity_current_)
    });
    float buff_item_y = 118;
    for (auto b : player_buffs_) {
      render_items.emplace_back(RenderInstanceItem{
        Transform{
          .position = {34, buff_item_y, 0},
          .size = {32, 32},
        },
        GetBuffIconUV(b.type), color::setOpacity(color::white, ui_opacity_current_)
      });

      buff_item_y += (32 + buff_item_gap);
    }
  }

  // Session: Left Bottom
  // Event log: Background
  if (is_show_event_log_) {
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {24, -194, 0},
        .size = {320, 170},
        .position_anchor = {0, static_cast<float>(ctx->window_height), 0}
      },
      texture_map["Block"], color::setOpacity(color::black, 0.25f * ui_opacity_current_)
    });
  }

  // Session: Center Upper
  // Timer: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-315 / 2, 24, 0},
      .size = {315, 45},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
    },
    texture_map["Block"], color::setOpacity(color::black, 0.25f * ui_opacity_current_)
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-315 / 2, 24, 0},
      .size = {315, 45},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
    },
    texture_map["TimerBackground"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

  if (is_showing_area_message_) {
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-166 / 2, 129 + 16, 0},
        .size = {166, 177 - (129 + 16)},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
      },
      texture_map["Block"], color::setOpacity(color::black, 0.1f * area_message_opacity_current_ * ui_opacity_current_)
    });
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-166 / 2, 129, 0},
        .size = {166, 16},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
      },
      texture_map["MessageUpper"], color::setOpacity(color::white, area_message_opacity_current_ * ui_opacity_current_)
    });
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-166 / 2, 177, 0},
        .size = {166, 16},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
      },
      texture_map["MessageLower"], color::setOpacity(color::white, area_message_opacity_current_ * ui_opacity_current_)
    });
  }

  // Session: Center Bottom
  // Attack Hint: Background
  constexpr float atk_hint_background_width = 122;
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-atk_hint_background_width / 2, -82 - 32, 0},
      .size = {atk_hint_background_width, 32},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["RoundBackground"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  // Attack Hint: Space bar
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-atk_hint_background_width / 2 + 72, -82 - 18 - 7, 0},
      .size = {36, 18},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSpaceUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

  // Skill Slot
  // Center
  constexpr float skill_slot_gap = 10.0f;
  if (is_show_skill_) {
    float skill_slot_width = static_cast<float>(48 * skill_count_ + (skill_count_ - 1) * skill_slot_gap);
    for (int i = 0; i < skill_count_; ++i) {
      POSITION skill_slot_position = {-skill_slot_width / 2 + i * (48 + skill_slot_gap), -24 - 48, 0};
      render_items.emplace_back(RenderInstanceItem{
        Transform{
          .position = skill_slot_position,
          .size = {48, 48},
          .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
        },
        texture_map["SkillSlot"],
        color::setOpacity(color::white, ui_opacity_current_)
      });

      render_items.emplace_back(RenderInstanceItem{
        Transform{
          .position = {skill_slot_position.x + 2, skill_slot_position.y + 2, 0},
          .size = {44, 44},
          .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
        },
        skill_uvs_[i],
        color::setOpacity(color::white, ui_opacity_current_)
      });

      if (skill_selected_ != i) continue;

      render_items.emplace_back(RenderInstanceItem{
        Transform{
          .position = {-skill_slot_width / 2 + i * (48 + skill_slot_gap), -24 - 48, 0},
          .size = {48, 48},
          .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
        },
        texture_map["SkillSelected"],
        color::setOpacity(color::white, ui_opacity_current_)
      });
    }

    // Skill switch hint
    constexpr float skill_hint_size = 24.0f;
    float left_skill_hint_x = -skill_slot_width / 2 - skill_slot_gap - skill_hint_size;
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {left_skill_hint_x, -36 - skill_hint_size, 0},
        .size = {24, 24},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
      },
      texture_map["KeyboardQ"],
      color::setOpacity(color::white, ui_opacity_current_)
    });

    float right_skill_hint_x = skill_slot_width / 2 + skill_slot_gap;
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {right_skill_hint_x, -36 - skill_hint_size, 0},
        .size = {24, 24},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
      },
      texture_map["KeyboardE"],
      color::setOpacity(color::white, ui_opacity_current_)
    });
  }

  // Session: Right Upper
  // Coin: Background
  if (is_show_coin_) {
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-24 - 182, 24, 0},
        .size = {182, 46},
        .position_anchor = {static_cast<float>(ctx->window_width), 0, 0}
      },
      texture_map["RoundBackground"],
      color::setOpacity(color::white, ui_opacity_current_)
    });
    // Coin: Icon
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-24 - 182 + 18, 32, 0},
        .size = {24, 28},
        .position_anchor = {static_cast<float>(ctx->window_width), 0, 0}
      },
      texture_map["Coin"],
      color::setOpacity(color::white, ui_opacity_current_)
    });
  }
  
  // Session: Upper
  // Experience Bar
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {0, 0, 0},
      .size = {static_cast<float>(ctx->window_width) * experience_bar_percentage_current_, 8}
    },
    texture_map["Block"],
    color::setOpacity(color::amberA700, ui_opacity_current_)
  });

  rr->DrawSpritesInstanced(render_items, texture_id_, {}, true);

  if (is_x_input_) {
    x_button_input_hints_->OnRender(ctx);
  } else {
    input_hint_->OnRender(ctx);
  }

  // On Top of instanced Draw
  // Session: Left Upper
  // HP Bar: Player Name
  rr->DrawFont(L"プレイヤー", font_key_,
               Transform{.position = {110, 36, 0}},
               StringSpriteProps{
                 .pixel_size = 14.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });

  // Session: Left Upper
  // HP Bar: Level
  rr->DrawFont(L"レベル", font_key_,
               Transform{.position = {40, 37, 0}},
               StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });
  wss.str(L"");
  wss << player_level_;

  auto level_text_props = StringSpriteProps{
    .pixel_size = 32.0f,
    .letter_spacing = 0.0f,
    .line_height = 0.0f,
    .color = color::setOpacity(color::white, ui_opacity_current_)
  };
  StringSpriteSize level_size = default_font_->GetStringSize(wss.str(), {}, level_text_props);

  rr->DrawFont(wss.str(), font_key_,
               Transform{.position = {64 - level_size.width / 2, 50, 0}}, level_text_props);

  // Session: Left Upper
  // Buff
  float buff_text_y = 126;
  for (auto b : player_buffs_) {
    wss.str(L"");
    wss << GetBuffDisplayText(b.type);
    wss << std::fixed << std::setprecision(1) << (b.duration - b.elapsed);
    rr->DrawFont(wss.str(), font_key_,
                 Transform{.position = {70, buff_text_y, 0}},
                 StringSpriteProps{
                   .pixel_size = 16.0f,
                   .letter_spacing = 0.0f,
                   .line_height = 0.0f,
                   .color = color::setOpacity(color::white, ui_opacity_current_)
                 });

    constexpr float buff_text_gap = 26;
    buff_text_y += 16 + buff_text_gap;
  }

  // Session: Left Bottom
  // Event log: log text
  // TODO: Assign color to different event
  if (is_show_event_log_) {
    wss.str(L"");
    wss.clear();
    for (auto text : text_list_) {
      wss << text.text << "\n";
    }
    auto text_props = StringSpriteProps{
      .pixel_size = 12.0f,
      .letter_spacing = 0.0f,
      .line_height = 22.0f,
      .color = color::setOpacity(color::white, ui_opacity_current_)
    };

    float box_margin = 24;
    float text_start_y = -box_margin - text_props.line_height * text_list_.size();

    constexpr float text_box_x = 22;
    float text_box_y = static_cast<float>(ctx->window_height) - 170 - box_margin;
    constexpr float text_box_height = 170;
    constexpr float text_box_width = 320;

    rr->SetScissorRect(text_box_x, text_box_y, text_box_x + text_box_width, text_box_y + text_box_height);
    for (int i = 0; i < text_list_.size(); i++) {
      auto& text = text_list_[i];

      float text_y = text_start_y + i * text_props.line_height;
      text_props.color = color::setOpacity(text.color, ui_opacity_current_);
      rr->DrawFont(text.text, font_key_,
                   Transform{
                     .position = {34, text_y, 0},
                     .position_anchor = {0, static_cast<float>(ctx->window_height), 0}
                   }, text_props);
    }
    rr->ResetScissorRect();
  }

  // Session: Center Upper
  // Timer: text
  StringSpriteSize timer_size = default_font_->GetStringSize(timer_text_, {}, {32.0f});
  rr->DrawFont(timer_text_, font_key_,
               Transform{
                 .position = {-timer_size.width / 2, 37, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
               }, StringSpriteProps{
                 .pixel_size = 32.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });

  if (is_showing_area_message_) {
    StringSpriteSize area_message_size = default_font_->GetStringSize(area_message_, {}, {20.0f});
    rr->DrawFont(area_message_, font_key_,
                 Transform{
                   .position = {-area_message_size.width / 2, 149, 0},
                   .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
                 }, StringSpriteProps{
                   .pixel_size = 20.0f,
                   .letter_spacing = 0.0f,
                   .line_height = 0.0f,
                   .color = color::setOpacity(color::white, ui_opacity_current_)
                 });
  }

  // Session: Center Bottom
  // Attack Hint: text
  rr->DrawFont(L"攻撃する", font_key_,
               Transform{
                 .position = {-122 / 2 + 17, -82 - 22, 0},
                 .position_anchor = {
                   static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0
                 }
               }, StringSpriteProps{
                 .pixel_size = 12.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 22.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });

  // Session: Right Upper
  // Coin: Text
  if (is_show_coin_) {
    wss.str(L"");
    wss << L"コイン " << coin_text_;

    rr->DrawFont(wss.str(), font_key_,
                 Transform{
                   .position = {-24 - 182 + 45, 35, 0},
                   .position_anchor = {static_cast<float>(ctx->window_width), 0, 0}
                 }, StringSpriteProps{
                   .pixel_size = 22.0f,
                   .letter_spacing = 0.0f,
                   .line_height = 0.0f,
                   .color = color::setOpacity(color::white, ui_opacity_current_)
                 });
  }

  RenderDamageText(ctx, scene_ctx, camera);
  RenderExperienceCoin(ctx, scene_ctx, camera);

  // Draw Event texts
  for (auto event_text : event_texts) {
    wss.str(L"");
    wss << event_text.text;

    auto text_props = StringSpriteProps{
      .pixel_size = 20.0f,
      .letter_spacing = 0.0f,
      .line_height = 0.0f,
      .color = color::setOpacity(event_text.color, event_text.opacity)
    };

    auto size = default_font_->GetStringSize(wss.str(), {}, text_props);

    Vector2 event_text_base_position = {
      static_cast<float>(ctx->window_width) / 2,
      static_cast<float>(ctx->window_height) - 250
    };

    rr->DrawFont(
      wss.str(),
      font_key_,
      Transform{
        .position = {
          event_text_base_position.x + event_text.position.x - size.width / 2,
          event_text_base_position.y + event_text.position.y - size.height / 2,
          0
        }
      }, text_props
    );
  }


  rr->DrawSprite(RenderItem{
                   fade_overlay_texture_id_,
                   Transform{
                     {0, 0, 0},
                     {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height)},
                   },
                   {{0, 0}, {8, 8}},
                   color::setOpacity(color::black, fade_overlay_alpha_current_)
                 }, {});
}

void GameUI::RenderDamageText(GameContext* ctx, SceneContext*, Camera* camera) {
  auto rr = ctx->render_resource_manager->renderer.get();
  std::wstringstream wss;

  for (auto damage_text : damage_texts) {
    wss.str(L"");
    wss << damage_text.skill_name << " " << std::to_wstring(damage_text.damage);

    auto text_props = StringSpriteProps{
      .pixel_size = 6.0f,
      .letter_spacing = 0.0f,
      .line_height = 0.0f,
      .color = color::setOpacity(color::yellow500, damage_text.opacity)
    };

    auto size = default_font_->GetStringSize(wss.str(), {}, text_props);

    rr->DrawFont(
      wss.str(),
      font_key_,
      Transform{
        .position = {damage_text.position.x - size.width / 2, damage_text.position.y - size.height / 2, 0}
      }, text_props,
      camera->GetCameraProps()
    );
  }
}

void GameUI::RenderExperienceCoin(GameContext* ctx, SceneContext*, Camera* camera) {
  auto rr = ctx->render_resource_manager->renderer.get();

  std::vector<RenderInstanceItem> render_items_on_map;
  std::vector<RenderInstanceItem> render_items_on_screen;
  std::vector<RenderInstanceItem> render_items_trajectory;

  for (auto exp_coin : experience_stars_) {
    if (exp_coin.floating_timeout > 0) {
      render_items_on_map.emplace_back(RenderInstanceItem{
        .transform = {
          .position = exp_coin.position,
          .size = {8, 8},
          .position_anchor = {-4, -4, 0}
        },
        .uv = texture_map["Star"],
        .color = color::white
      });
    }
    else {
      render_items_on_screen.emplace_back(RenderInstanceItem{
        .transform = {
          .position = exp_coin.position,
          .size = {24, 24},
          .position_anchor = {-12, -12, 0}
        },
        .uv = texture_map["Star"],
        .color = color::white
      });
    }
  }

  for (auto s : experience_stars_trajectory_) {
    render_items_trajectory.emplace_back(RenderInstanceItem{
      .transform = {
        .position = s.position,
        .size = {s.size, s.size},
        .position_anchor = {-s.size / 2, -s.size / 2, 0}
      },
      .uv = texture_map["StarAdditive"],
      .color = color::setOpacity(color::yellowA200, 0.3f)
    });
  }

  for (auto s : experience_stars_end_) {
    render_items_trajectory.emplace_back(RenderInstanceItem{
      .transform = {
        .position = s.position,
        .size = {s.size, s.size},
        .position_anchor = {-s.size / 2, -s.size / 2, 0}
      },
      .uv = texture_map["StarAdditive"],
      .color = color::setOpacity(color::yellowA200, s.opacity)
    });
  }

  rr->SetAdditiveBlending();
  rr->DrawSpritesInstanced(render_items_trajectory, texture_id_, {}, true);
  rr->SetMultiplicativeBlending();

  rr->DrawSpritesInstanced(render_items_on_map, texture_id_, camera->GetCameraProps(), true);
  rr->DrawSpritesInstanced(render_items_on_screen, texture_id_, {}, true);
}
