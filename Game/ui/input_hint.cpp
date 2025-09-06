module;

module game.ui.component.input_hint;

InputHintComponent::InputHintComponent(GameContext* ctx, const InputHintProps& props):
  opacity_(props.opacity), left_aligned_(props.left_aligned), top_aligned_(props.top_aligned) {
  // Texture
  auto [font_spritemap_filename, font_metadata_filename] = defined_font_map[DefinedFont::FUSION_PIXEL_FONT_DEBUG];
  font_key_ = Font::MakeFontGetKey(font_spritemap_filename, font_metadata_filename,
                                   ctx->render_resource_manager->texture_manager.get());
  default_font_ = Font::GetFont(font_key_);
  auto& tm = ctx->render_resource_manager->texture_manager;
  ui_texture_id_ = tm->Load(L"assets/ui.png");

  // Init input hints cache
  for (const auto& input_hint : props.input_hints) {
    input_hints.emplace_back(DisplayInputHint{
      .label = input_hint.label,
      .keys = input_hint.keys,
      .uvs = input_hint.keys
      | std::views::transform([](const auto& key) {
        if (auto k_code = std::get_if<KeyCode>(&key)) {
          return keyboard_key_to_uv[*k_code];
        }
        if (auto x_code = std::get_if<XButtonCode>(&key)) {
          return xbutton_key_to_uv[*x_code];
        }
        return UV{};
      })
      | std::ranges::to<std::vector>(),
      .on_press_uvs = input_hint.keys
      | std::views::transform([](const auto& key) {
        if (auto k_code = std::get_if<KeyCode>(&key)) {
          return keyboard_key_on_press_to_uv[*k_code];
        }
        if (auto x_code = std::get_if<XButtonCode>(&key)) {
          return xbutton_key_on_press_to_uv[*x_code];
        }
        return UV{};
      })
      | std::ranges::to<std::vector>(),
      .is_pressed = std::vector<bool>(input_hint.keys.size(), false)
    });
  }
}

void InputHintComponent::OnUpdate(GameContext* ctx, float) {
  auto& input_handler = ctx->input_handler;
  for (DisplayInputHint& input_hint : input_hints) {
    for (int i = 0; i < input_hint.keys.size(); ++i) {
      auto key = input_hint.keys[i];
      if (const auto k_code = std::get_if<KeyCode>(&key); input_handler->GetKey(*k_code)) {
        input_hint.is_pressed[i] = true;
      }
      else if (const auto x_code = std::get_if<XButtonCode>(&key);
        input_handler->IsXInputConnected() && input_handler->GetXInputButton(*x_code)) {
        input_hint.is_pressed[i] = true;
      }
      else {
        input_hint.is_pressed[i] = false;
      }
    }
  }
}

void InputHintComponent::OnFixedUpdate(GameContext* ctx, float delta_time) {}

void InputHintComponent::OnRender(GameContext* ctx) {
  auto& rr = ctx->render_resource_manager->renderer;

  std::vector<RenderInstanceItem> items;

  // Background
  constexpr UV background_uv = UV{{96, 297}, {8, 8}};
  float background_height = box_padding * 2 + row_margin_top + (key_size + row_gap) * input_hints.size();
  float background_width = 224;

  float box_x = left_aligned_ ? box_margin : static_cast<float>(ctx->window_width) - background_width - box_margin;
  float box_y = top_aligned_ ? box_margin : static_cast<float>(ctx->window_height) - background_height - box_margin;

  items.emplace_back(RenderInstanceItem{
    Transform{
      {box_x, box_y, 0},
      {background_width, background_height},
    },
    background_uv,
    color::setOpacity(color::black, opacity_ * 0.25f)
  });

  // Left Upper Decal
  constexpr float decal_margin = 8;
  constexpr UV decal_uv = UV{{129, 242}, {33, 15}};

  items.emplace_back(RenderInstanceItem{
    Transform{
      .position = {box_x + decal_margin, box_y + decal_margin, 0},
      .size = {49, 22},
    },
    decal_uv,
    color::setOpacity(color::white, opacity_)
  });

  rr->DrawSpritesInstanced(items, ui_texture_id_, {}, true);

  // Texts
  items.clear();
  auto label_text_props = StringSpriteProps{
    .pixel_size = 18.0f,
    .line_height = key_size,
    .color = color::setOpacity(color::white, opacity_)
  };

  std::wstringstream wss;
  for (int i = 0; i < input_hints.size(); i++) {
    DisplayInputHint hint = input_hints[i];

    float row_x = box_x + box_padding + row_padding_left;
    float row_y = box_y + box_padding + row_margin_top + (key_size + row_gap) * i;

    wss.str(L"");
    wss << hint.label;
    rr->DrawFont(wss.str(), font_key_, {
                   Transform{
                     {
                       row_x,
                       row_y + (key_size - label_text_props.pixel_size) / 2 - 3,
                       0
                     }
                   }
                 }, label_text_props);

    // keys
    std::vector<UV> key_uvs = input_hints[i].uvs;

    StringSpriteSize label_size = default_font_->GetStringSize(wss.str(), {}, label_text_props);
    for (int j = 0; j < input_hints.size(); j++) {
      // UV key_uv = key_uvs[j];
      UV key_uv = input_hints[i].is_pressed[j] ? input_hints[i].on_press_uvs[j] : input_hints[i].uvs[j];

      // Key width is based on the height of the key
      float key_width = key_size * key_uv.size.x / key_uv.size.y;

      items.emplace_back(RenderInstanceItem{
        Transform{
          .position = {row_x + label_size.width + keys_left_padding + j * (key_width + row_gap), row_y, 0},
          .size = {key_width, key_size},
        },
        key_uv,
        color::setOpacity(color::white, opacity_)
      });
    }
  }

  rr->DrawSpritesInstanced(items, ui_texture_id_, {}, true);
}
