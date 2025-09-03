module;

module game.ui.title_ui;

import std;
import game.ui.interpolation;

TitleUI::TitleUI(GameContext* ctx)
{
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
}

void TitleUI::OnUpdate(GameContext*, float delta_time)
{
    uv_horizontal_offset_ += delta_time * uv_offset_speed_;

    movement_acc_ += delta_time;

    fade_overlay_alpha_current_ = interpolation::UpdateSmoothValue(
        fade_overlay_alpha_current_,
        fade_overlay_alpha_target_,
        delta_time,
        interpolation::SmoothType::EaseOut,
        1.0f
    );

    if (fade_overlay_callback_)
    {
        float diff = std::fabs(fade_overlay_alpha_target_ - fade_overlay_alpha_current_);
        if (diff < 0.01f)
        {
            auto cb = fade_overlay_callback_;
            fade_overlay_callback_ = {};
            cb();
        }
    }
}

void TitleUI::OnFixedUpdate(GameContext*, float)
{
}

void TitleUI::OnRender(GameContext* ctx, Camera*)
{
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
        color::white
    });

    rr->DrawSprite(RenderItem{
        title_texture_id_,
        Transform{
            {
                static_cast<float>(ctx->window_width) / 2 - 225,
                static_cast<float>(ctx->window_height) / 2 - 280 +
                std::cos(1.5f * movement_acc_) * title_logo_floating_speed_,
                0
            },
            {450, 300}
        },
        {{0, 0}, {300, 200}},
        color::white
    });

    // Start Button
    float start_button_center_x = static_cast<float>(ctx->window_width) / 2;
    float start_button_center_y = static_cast<float>(ctx->window_height) / 2 + 125;

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

    // End Button
    float end_button_center_x = static_cast<float>(ctx->window_width) / 2;
    float end_button_center_y = static_cast<float>(ctx->window_height) / 2 + 220;

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
                end_button_center_x - 256 / 2,
                end_button_center_y - 64 / 2,
                0
            },
            {256, 64}
        },
        {{frame_uv_pos_.x, frame_uv_pos_.y}, {frame_uv_size_.x, frame_uv_size_.y}},
        color::white
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
        color::white
    });

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
