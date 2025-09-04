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

  std::erase_if(experience_stars_, [target = EXP_COIN_TARGET_POS](const ExperienceStar& exp_star) {
    return math::GetDistance({exp_star.position.x, exp_star.position.y}, target) <= 2;
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

  std::erase_if(experience_stars_trajectory_, [](const StarTrajectory& t) {
    return t.size <= 1;
  });

  for (auto& t : experience_stars_trajectory_) {
    t.size = interpolation::UpdateSmoothValue(
      t.size,
      0,
      delta_time,
      interpolation::SmoothType::EaseOut,
      3.0f
    );
  }
}

void GameUI::OnFixedUpdate(GameContext*, SceneContext*, float delta_time) {
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
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {38, 30, 0},
      .size = {52, 52},
    },
    texture_map["Heart"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
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
  int item_count = player_buffs_.size();
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

  // Session: Right Bottom
  // Input Hint: Background
  constexpr float hint_box_margin = 24;

  constexpr float hint_box_width = 224;
  constexpr float hint_box_height = 122;
  constexpr float hint_box_x = -hint_box_margin - hint_box_width;
  constexpr float hint_box_y = -hint_box_margin - hint_box_height;

  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-hint_box_margin - hint_box_width, -hint_box_margin - hint_box_height, 0},
      .size = {hint_box_width, hint_box_height},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Block"], color::setOpacity(color::black, 0.25f * ui_opacity_current_)
  });
  // Input Hint: Corner
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-hint_box_margin - hint_box_width + 8, -hint_box_margin - hint_box_height + 8, 0},
      .size = {49, 22},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Corner"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  // Input Hint: Key
  constexpr float key_left_padding = 106;
  constexpr float key_gap = 8;
  constexpr float key_size = 20;

  const std::vector<UV> moving_keys = {
    texture_map["KeyboardWUp"],
    texture_map["KeyboardAUp"],
    texture_map["KeyboardSUp"],
    texture_map["KeyboardDUp"],
  };

  for (int i = 0; i < moving_keys.size(); i++) {
    constexpr float moving_keys_y = -hint_box_margin - hint_box_height + 50;

    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-hint_box_margin - hint_box_width + key_left_padding + i * (key_size + key_gap), moving_keys_y, 0},
        .size = {20, 20},
        .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
      },
      moving_keys[i],
      color::setOpacity(color::white, ui_opacity_current_)
    });
  }

  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-hint_box_margin - hint_box_width + key_left_padding, -hint_box_margin - hint_box_height + 74, 0},
      .size = {40, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSpaceUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

  const std::vector<UV> skill_switch = {
    texture_map["KeyboardQ"],
    texture_map["KeyboardE"],
  };

  for (int i = 0; i < skill_switch.size(); i++) {
    constexpr float skill_switch_y = -hint_box_margin - hint_box_height + 98;

    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-hint_box_margin - hint_box_width + 122 + i * (key_size + key_gap), skill_switch_y, 0},
        .size = {20, 20},
        .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
      },
      skill_switch[i],
      color::setOpacity(color::white, ui_opacity_current_)
    });
  }

  rr->DrawSpritesInstanced(render_items, texture_id_, {}, true);

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

  // Session: Right Bottom
  // Hint: text
  constexpr float hint_box_left_padding = 32;
  constexpr float hint_box_top_padding = 26;
  constexpr float line_size = 16.0f;
  constexpr float line_gap = 8;

  const std::array<std::wstring, 4> instruction_texts = {
    L"操作説明",
    L"移動する",
    L"攻撃する",
    L"スキル選択",
  };

  for (int i = 0; i < instruction_texts.size(); i++) {
    rr->DrawFont(instruction_texts[i], font_key_,
                 Transform{
                   .position = {
                     hint_box_x + hint_box_left_padding,
                     hint_box_y + (line_size * i) + (line_gap * i) + hint_box_top_padding, 0
                   },
                   .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
                 }, StringSpriteProps{
                   .pixel_size = line_size,
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

void GameUI::RenderExperienceCoin(GameContext* ctx, SceneContext* scene_ctx, Camera* camera) {
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
          .position_anchor = {-4, -4, 0}
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
        .position_anchor = {-4, -4, 0}
      },
      .uv = texture_map["StarAdditive"],
      .color = color::setOpacity(color::yellowA200, 0.3f)
    });
  }

  rr->SetAdditiveBlending();
  rr->DrawSpritesInstanced(render_items_trajectory, texture_id_, {}, true);
  rr->SetMultiplicativeBlending();

  rr->DrawSpritesInstanced(render_items_on_map, texture_id_, camera->GetCameraProps(), true);
  rr->DrawSpritesInstanced(render_items_on_screen, texture_id_, {}, true);
}
