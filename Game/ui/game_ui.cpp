module;

module game.ui.game_ui;

import std;
import graphic.utils.types;
import graphic.utils.font;

GameUI::GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path) {
  texture_id_ = ctx->render_resource_manager->texture_manager->Load(texture_path);

  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);
}

void GameUI::OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {}

void GameUI::OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {}

void GameUI::OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera) {
  auto& rr = ctx->render_resource_manager->renderer;
  std::wstringstream wss;

  std::vector<RenderInstanceItem> render_items;

  // Session: Left Upper
  // HP Bar: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {24, 24, 0},
      .size = {320, 64},
    },
    texture_map["HPBarPanel"], color::white
  });
  // HP Bar: Heart Background Circle
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {28, 20, 0},
      .size = {72, 72},
    },
    texture_map["BigCircle"], color::white
  });
  // HP Bar: Heart
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {38, 30, 0},
      .size = {52, 52},
    },
    texture_map["Heart"], color::white
  });
  // HP Bar: HB Bar
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {110, 59, 0},
      .size = {200 / hp_percentage_, 14},
    },
    texture_map["HPBar"], color::white
  });
  // HP Bar: HB Bar Frame
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {106, 54, 0},
      .size = {208, 24},
    },
    texture_map["HPBarFrame"], color::white
  });

  // Session: Left Bottom
  // Event log: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {24, -204, 0},
      .size = {320, 180},
      .position_anchor = {0, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Block"], color::setOpacity(color::black, 0.25f)
  });

  // Session: Center Upper
  // Timer: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-315 / 2, 24, 0},
      .size = {315, 45},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, 0, 0}
    },
    texture_map["TimerBackground"], color::white
  });

  // Session: Center Bottom
  // Attack Hint: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-122 / 2, -82 - 32, 0},
      .size = {122, 32},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["RoundBackground"], color::white
  });
  // Attack Hint: Space bar
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-122 / 2 + 73, -82 - 24, 0},
      .size = {32, 16},
      .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSpaceUp"], color::white
  });

  // Skill Slot
  // Center
  float skill_slot_width = 48 * skill_count_ + (skill_count_ - 1) * 10;
  for (int i = 0; i < skill_count_; ++i) {
    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-skill_slot_width / 2 + i * (48 + 10), -24 - 48, 0},
        .size = {48, 48},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
      },
      texture_map["SkillSlot"], color::white
    });

    if (skill_selected_ != i) continue;

    render_items.emplace_back(RenderInstanceItem{
      Transform{
        .position = {-skill_slot_width / 2 + i * (48 + 10), -24 - 48, 0},
        .size = {48, 48},
        .position_anchor = {static_cast<float>(ctx->window_width) / 2, static_cast<float>(ctx->window_height), 0}
      },
      texture_map["SkillSelected"], color::white
    });
  }

  // Session: Right Upper
  // Coin: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 182, 24, 0},
      .size = {182, 46},
      .position_anchor = {static_cast<float>(ctx->window_width), 0, 0}
    },
    texture_map["RoundBackground"], color::white
  });
  // Coin: Icon
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 182 + 18, 32, 0},
      .size = {24, 28},
      .position_anchor = {static_cast<float>(ctx->window_width), 0, 0}
    },
    texture_map["Coin"], color::white
  });

  // Session: Right Bottom
  // Input Hint: Background
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224, -24 - 98, 0},
      .size = {224, 98},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Block"], color::setOpacity(color::black, 0.25f)
  });
  // Input Hint: Corner
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 8, -24 - 98 + 8, 0},
      .size = {49, 22},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["Corner"], color::white
  });
  // Input Hint: Key
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 106, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardWUp"], color::white
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 126, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardAUp"], color::white
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 146, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSUp"], color::white
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 166, -24 - 98 + 50, 0},
      .size = {20, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardDUp"], color::white
  });
  render_items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {-24 - 224 + 106, -24 - 98 + 74, 0},
      .size = {40, 20},
      .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
    },
    texture_map["KeyboardSpaceUp"], color::white
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
                 .line_height = 0.0f
               });

  // Session: Left Bottom
  // Event log: log text
  // TODO: Assign color to different event
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
                 .line_height = 22.0f
               });

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
                 .color = color::white
               });

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
                 .line_height = 22.0f
               });

  // Session: Right Upper
  // Coin: Text
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
                 .color = color::white
               });

  rr->DrawFont(L"操作説明", font_key_,
               Transform{
                 .position = {-24 - 224 + 32, -24 - 98 + 26, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
               }, StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::white
               });
  rr->DrawFont(L"移動する", font_key_,
               Transform{
                 .position = {-24 - 224 + 32, -24 - 98 + 50, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
               }, StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::white
               });
  rr->DrawFont(L"攻撃する", font_key_,
               Transform{
                 .position = {-24 - 224 + 32, -24 - 98 + 74, 0},
                 .position_anchor = {static_cast<float>(ctx->window_width), static_cast<float>(ctx->window_height), 0}
               }, StringSpriteProps{
                 .pixel_size = 16.0f,
                 .letter_spacing = 0.0f,
                 .line_height = 0.0f,
                 .color = color::white
               });
}
