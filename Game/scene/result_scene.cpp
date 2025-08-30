module;

module game.result_scene;

import game.audio.audio_clip;
import game.scene_manager;
import game.scene_game;
import game.title_scene;

ResultScene::ResultScene(ResultSceneProps props) {
  monster_killed_ = props.monster_killed;
  
  int total_seconds = static_cast<int>(props.elapsed_seconds);
  minutes_ = (total_seconds % 3600) / 60;
  seconds_ = total_seconds % 60;
}

void ResultScene::OnEnter(GameContext* ctx) {
  result_ui_ = std::make_unique<ResultUI>(ctx);
  result_ui_->SetMinutes(minutes_);
  result_ui_->SetSeconds(seconds_);
  result_ui_->SetMonsterKilled(monster_killed_);
}

void ResultScene::OnUpdate(GameContext* ctx, float delta_time) {
  auto am = SceneManager::GetInstance().GetAudioManager();

  if (is_allow_control_) {
    if ((ctx->input_handler->GetKey(KeyCode::KK_UP) || ctx->input_handler->GetKey(KeyCode::KK_W))
      && input_throttle_.CanCall()) {
      selected_option_++;
      selected_option_ %= options_count;
      result_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((ctx->input_handler->GetKey(KeyCode::KK_DOWN) || ctx->input_handler->GetKey(KeyCode::KK_S))
      && input_throttle_.CanCall()) {
      selected_option_--;
      selected_option_ += options_count;
      selected_option_ %= options_count;
      result_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((ctx->input_handler->IsKeyDown(KeyCode::KK_ENTER) || ctx->input_handler->IsKeyDown(KeyCode::KK_SPACE))
      && enter_throttle_.CanCall()) {
      if (static_cast<SelectedOption>(selected_option_) == SelectedOption::RESTART) {
        am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
        result_ui_->SetFadeOverlayAlphaTarget(1.0f, color::black, []() -> void {
          SceneManager::GetInstance().ChangeScene(std::make_unique<GameScene>());
        });
      }
      else if (static_cast<SelectedOption>(selected_option_) == SelectedOption::BACK_TO_TITLE) {
        am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
        result_ui_->SetFadeOverlayAlphaTarget(1.0f, color::white, []() -> void {
          SceneManager::GetInstance().ChangeScene(std::make_unique<TitleScene>());
        });
      }
    }
  }

  result_ui_->OnUpdate(ctx, delta_time);
}

void ResultScene::OnFixedUpdate(GameContext* ctx, float delta_time) {
  result_ui_->OnFixedUpdate(ctx, delta_time);
}

void ResultScene::OnRender(GameContext* ctx) {
  result_ui_->OnRender(ctx, {});
}

void ResultScene::OnExit(GameContext* ctx) {}
