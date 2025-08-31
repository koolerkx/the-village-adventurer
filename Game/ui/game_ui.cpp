module;

module game.ui.game_ui;

import std;
import graphic.utils.types;
import graphic.utils.font;
import game.ui.interpolation;

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

void GameUI::OnUpdate(GameContext*, SceneContext*, float delta_time) {
  hp_percentage_current_ = interpolation::UpdateSmoothValue(
    hp_percentage_current_,
    hp_percentage_target_,
    delta_time,
    interpolation::SmoothType::EaseOut,
    0.5f
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
    0.5f
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

  // Session: Left Bottom
  // Event log: Background
  if (is_show_event_log_) {
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {24, -204, 0},
        .size = {320, 180},
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
    texture_map["TimerBackground"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

  if (is_showing_area_message_) {
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
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-122 / 2, -82 - 32, 0},
      .size = {122, 32},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["RoundBackground"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  // Attack Hint: Space bar
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-122 / 2 + 73, -82 - 24, 0},
      .size = {32, 16},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSpaceUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

  // Skill Slot
  // Center
  if (is_show_skill_) {
    float skill_slot_width = static_cast<float>(48 * skill_count_ + (skill_count_ - 1) * 10);
    for (int i = 0; i < skill_count_; ++i) {
      POSITION skill_slot_position = {-skill_slot_width / 2 + i * (48 + 10), -24 - 48, 0};
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
          .position = {-skill_slot_width / 2 + i * (48 + 10), -24 - 48, 0},
          .size = {48, 48},
          .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
        },
        texture_map["SkillSelected"],
        color::setOpacity(color::white, ui_opacity_current_)
      });
    }
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
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224, -24 - 98, 0},
      .size = {224, 98},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Block"], color::setOpacity(color::black, 0.25f * ui_opacity_current_)
  });
  // Input Hint: Corner
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 8, -24 - 98 + 8, 0},
      .size = {49, 22},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Corner"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  // Input Hint: Key
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 106, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardWUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 126, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardAUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 146, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 166, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardDUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 106, -24 - 98 + 74, 0},
      .size = {40, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSpaceUp"],
    color::setOpacity(color::white, ui_opacity_current_)
  });

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

  // Session: Left Bottom
  // Event log: log text
  // TODO: Assign color to different event
  if (is_show_event_log_) {
    wss.str(L"");
    wss.clear();
    for (auto text : text_list_) {
      wss << text << "\n";
    }
    rr->DrawFont(wss.str(), font_key_,
                 Transform{
                   .position = {34, -194, 0},
                   .position_anchor = {0, static_cast<float>(ctx->window_height), 0}
                 }, StringSpriteProps{
                   .pixel_size = 12.0f,
                   .letter_spacing = 0.0f,
                   .line_height = 22.0f,
                   .color = color::setOpacity(color::white, ui_opacity_current_)
                 });
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
  rr->DrawFont(L"操作説明", font_key_,
               Transform{
                 .position = {-24 - 224 + 32, -24 - 98 + 26, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
               }, StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });
  rr->DrawFont(L"移動する", font_key_,
               Transform{
                 .position = {-24 - 224 + 32, -24 - 98 + 50, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
               }, StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });
  rr->DrawFont(L"攻撃する", font_key_,
               Transform{
                 .position = {-24 - 224 + 32, -24 - 98 + 74, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
               }, StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::setOpacity(color::white, ui_opacity_current_)
               });

  RenderDamageText(ctx, scene_ctx, camera);

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
