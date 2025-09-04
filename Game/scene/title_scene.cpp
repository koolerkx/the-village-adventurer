module;

module game.title_scene;

import std;
import graphic.utils.types;
import game.scene_manager;
import game.scene_game;
import game.audio.audio_clip;

void TitleScene::OnEnter(GameContext* ctx) {
  std::cout << "TitleScene> OnEnter" << std::endl;
  title_ui_ = std::make_unique<TitleUI>(ctx);

  SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_title);
}

void TitleScene::OnUpdate(GameContext* ctx, float delta_time) {
  title_ui_->OnUpdate(ctx, delta_time);
  auto am = SceneManager::GetInstance().GetAudioManager();

  auto& ih = ctx->input_handler;

  bool is_xinput_button_yes = ih->GetXInputButton(XButtonCode::A);
  bool is_xinput_button_up = ih->GetXInputButton(XButtonCode::DPadUp) || ih->GetXInputAnalog().first.second > 0.0f;
  bool is_xinput_button_down = ih->GetXInputButton(XButtonCode::DPadDown) || ih->GetXInputAnalog().first.second < 0.0f;

  if (is_allow_control_) {
    if ((ctx->input_handler->GetKey(KeyCode::KK_UP) || ctx->input_handler->GetKey(KeyCode::KK_W) || is_xinput_button_up)
      && input_throttle_.CanCall()) {
      selected_option_++;
      selected_option_ %= static_cast<uint8_t>(options_count);
      title_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((ctx->input_handler->GetKey(KeyCode::KK_DOWN) || ctx->input_handler->GetKey(KeyCode::KK_S) || is_xinput_button_down)
      && input_throttle_.CanCall()) {
      selected_option_--;
      selected_option_ += static_cast<uint8_t>(options_count);
      selected_option_ %= static_cast<uint8_t>(options_count);
      title_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((ctx->input_handler->IsKeyDown(KeyCode::KK_ENTER) || ctx->input_handler->IsKeyDown(KeyCode::KK_SPACE) || is_xinput_button_yes)
      && enter_throttle_.CanCall()) {
      if (static_cast<SelectedOption>(selected_option_) == SelectedOption::START_GAME) {
        am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
        title_ui_->SetFadeOverlayAlphaTarget(1.0f, color::black, [&is_allow_control = is_allow_control_]() -> void {
          SceneManager::GetInstance().ChangeSceneDelayed(std::make_unique<GameScene>());
          is_allow_control = false;
        });
      }
      else if (static_cast<SelectedOption>(selected_option_) == SelectedOption::END_GAME) {
        am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
        SceneManager::GetInstance().SetLeave(true);
      }
    }
  }
}

void TitleScene::OnFixedUpdate(GameContext* ctx, float delta_time) {
  title_ui_->OnFixedUpdate(ctx, delta_time);
}

void TitleScene::OnRender(GameContext* ctx) {
  title_ui_->OnRender(ctx, {});
}

void TitleScene::OnExit(GameContext*) {
  std::cout << "TitleScene> OnExit" << std::endl;
}
