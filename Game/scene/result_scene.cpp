module;

module game.result_scene;

import game.audio.audio_clip;
import game.scene_manager;
import game.scene_game;
import game.title_scene;

ResultScene::ResultScene(ResultSceneProps props) {
  monster_killed_ = props.monster_killed;
  level_ = props.level;

  int total_seconds = static_cast<int>(props.elapsed_seconds);
  minutes_ = (total_seconds % 3600) / 60;
  seconds_ = total_seconds % 60;

  score_ =
    monster_killed_ * score_multiplier_monster
    + level_ * score_multiplier_level
    + minutes_ * score_multiplier_time;
}

void ResultScene::OnEnter(GameContext* ctx) {
  SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_result);

  result_ui_ = std::make_unique<ResultUI>(ctx);
  result_ui_->SetMinutes(static_cast<float>(minutes_));
  result_ui_->SetSeconds(static_cast<float>(seconds_));
  result_ui_->SetMonsterKilled(monster_killed_);
  result_ui_->SetLevel(level_);
  result_ui_->SetMultiplierMonster(score_multiplier_monster);
  result_ui_->SetMultiplierLevel(score_multiplier_level);
  result_ui_->SetMultiplierTime(score_multiplier_time);
  result_ui_->SetScore(score_);
}

void ResultScene::OnUpdate(GameContext* ctx, float delta_time) {
  auto am = SceneManager::GetInstance().GetAudioManager();

  auto& ih = ctx->input_handler;

  bool is_xinput_button_yes = ih->GetXInputButton(XButtonCode::A);
  bool is_xinput_button_up = ih->GetXInputButton(XButtonCode::DPadUp) || ih->GetXInputAnalog().first.second > 0.0f;
  bool is_xinput_button_down = ih->GetXInputButton(XButtonCode::DPadDown) || ih->GetXInputAnalog().first.second < 0.0f;

  if (is_allow_control_) {
    if ((ih->GetKey(KeyCode::KK_UP) || ih->GetKey(KeyCode::KK_W) || is_xinput_button_up)
      && input_throttle_.CanCall()) {
      selected_option_++;
      selected_option_ %= options_count;
      result_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((ih->GetKey(KeyCode::KK_DOWN) || ih->GetKey(KeyCode::KK_S) || is_xinput_button_down)
      && input_throttle_.CanCall()) {
      selected_option_--;
      selected_option_ += static_cast<uint8_t>(options_count);
      selected_option_ %= static_cast<uint8_t>(options_count);
      result_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((ih->IsKeyDown(KeyCode::KK_ENTER) || ih->IsKeyDown(KeyCode::KK_SPACE) || is_xinput_button_yes)
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

void ResultScene::OnExit(GameContext*) {}
