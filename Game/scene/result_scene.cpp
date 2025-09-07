module;

module game.result_scene;

import game.audio.audio_clip;
import game.scene_manager;
import game.scene_game;
import game.title_scene;
import game.types;

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

  ranking_ = LoadRanking();
  ranking_.push_back(RankingItem{
    .score = static_cast<std::uint32_t>(score_),
    .timestamp_ms = NowEpochMillis(),
  });
  SortRanking(ranking_);
  SaveRanking(ranking_);

  for (auto r : ranking_) {
    std::cout << r.timestamp_ms << " " << r.score << std::endl;
  }

  is_x_input_ = props.is_default_x_input;
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
  result_ui_->SetRanking(ranking_);
}

void ResultScene::OnUpdate(GameContext* ctx, float delta_time) {
  auto am = SceneManager::GetInstance().GetAudioManager();

  auto& ih = ctx->input_handler;

  bool is_xinput_button_yes = ih->GetXInputButton(XButtonCode::A);
  bool is_xinput_button_up = ih->GetXInputButton(XButtonCode::DPadUp) || ih->GetXInputAnalog().first.second > 0.0f;
  bool is_xinput_button_down = ih->GetXInputButton(XButtonCode::DPadDown) || ih->GetXInputAnalog().first.second < 0.0f;
  bool is_xinput_any = is_xinput_button_up || is_xinput_button_down || is_xinput_button_yes;

  bool is_keyboard_yes = ih->GetKey(KeyCode::KK_ENTER) || ih->GetKey(KeyCode::KK_SPACE);
  bool is_keyboard_up = ih->GetKey(KeyCode::KK_UP) || ih->GetKey(KeyCode::KK_W);
  bool is_keyboard_down = ih->GetKey(KeyCode::KK_DOWN) || ih->GetKey(KeyCode::KK_S);
  bool is_keyboard_any = is_keyboard_up || is_keyboard_down || is_keyboard_yes;

  if (is_allow_control_) {
    if ((is_keyboard_up || is_xinput_button_up) && input_throttle_.CanCall()) {
      selected_option_++;
      selected_option_ %= options_count;
      result_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((is_keyboard_down || is_xinput_button_down) && input_throttle_.CanCall()) {
      selected_option_--;
      selected_option_ += static_cast<uint8_t>(options_count);
      selected_option_ %= static_cast<uint8_t>(options_count);
      result_ui_->SetSelectedOption(selected_option_);
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }

    if ((is_keyboard_yes || is_xinput_button_yes) && enter_throttle_.CanCall()) {
      if (static_cast<SelectedOption>(selected_option_) == SelectedOption::RESTART) {
        am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
        result_ui_->SetFadeOverlayAlphaTarget(1.0f, color::black, [&is_x_input = is_x_input_]() -> void {
          SceneManager::GetInstance().ChangeSceneDelayed(std::make_unique<GameScene>(is_x_input));
        });
      }
      else if (static_cast<SelectedOption>(selected_option_) == SelectedOption::BACK_TO_TITLE) {
        am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
        result_ui_->SetFadeOverlayAlphaTarget(1.0f, color::white, [&is_x_input = is_x_input_]() -> void {
          SceneManager::GetInstance().ChangeSceneDelayed(std::make_unique<TitleScene>(is_x_input));
        });
      }
    }
  }

  if (is_keyboard_any) {
    result_ui_->SetIsXInput(false);
    is_x_input_ = false;
  }
  else if (is_xinput_any) {
    is_x_input_ = true;
  }
  result_ui_->SetIsXInput(is_x_input_);

  result_ui_->OnUpdate(ctx, delta_time);
}

void ResultScene::OnFixedUpdate(GameContext* ctx, float delta_time) {
  result_ui_->OnFixedUpdate(ctx, delta_time);
}

void ResultScene::OnRender(GameContext* ctx) {
  result_ui_->OnRender(ctx, {});
}

void ResultScene::OnExit(GameContext*) {}

// Recreate the file if the save data is crash
std::vector<RankingItem> ResultScene::LoadRanking(const std::string& filepath) {
  std::vector<RankingItem> items;

  // Create empty
  if (!std::filesystem::exists(filepath)) {
    CreateEmptyRankingFile();
    return {};
  }

  std::ifstream ifs(filepath, std::ios::binary);
  if (!ifs) {
    std::cerr << "Failed to open file for reading: " << filepath << "\n";
    CreateEmptyRankingFile();
    return {};
  }

  std::uint32_t count = 0;
  if (!ifs.read(reinterpret_cast<char*>(&count), sizeof(count))) {
    std::cerr << "Failed to read count from file: " << filepath << "\n";
    CreateEmptyRankingFile();
    return {};
  }

  items.resize(count);
  if (count > 0) {
    const std::size_t bytes = static_cast<std::size_t>(count) * sizeof(RankingItem);
    if (!ifs.read(reinterpret_cast<char*>(items.data()), static_cast<std::streamsize>(bytes))) {
      std::cerr << "Failed to read items from file: " << filepath << "\n";
      items.clear();
      CreateEmptyRankingFile();
    }
  }

  return items;
}


void ResultScene::SaveRanking(const std::vector<RankingItem>& items, const std::string& filepath) {
  std::ofstream ofs(filepath, std::ios::binary | std::ios::trunc);
  if (!ofs) {
    throw std::runtime_error("Failed to open file for writing: " + filepath);
  }

  const std::uint32_t count = static_cast<std::uint32_t>(items.size());
  ofs.write(reinterpret_cast<const char*>(&count), sizeof(count));

  if (count > 0) {
    const std::size_t bytes = static_cast<std::size_t>(count) * sizeof(RankingItem);
    ofs.write(reinterpret_cast<const char*>(items.data()), static_cast<std::streamsize>(bytes));
  }
}

void ResultScene::CreateEmptyRankingFile(const std::string& filepath) {
  std::ofstream ofs(filepath, std::ios::binary);
  if (!ofs) {
    std::cerr << "Failed to create file: " << filepath << "\n";
    return;
  }
  const std::uint32_t count = 0;
  ofs.write(reinterpret_cast<const char*>(&count), sizeof(count));
}
