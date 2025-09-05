module;
#include <time.h>
#include "stdint.h"

export module game.ui.result_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;
import game.types;

export class ResultUI {
private:
  std::wstring font_key_;
  Font* default_font_;

  FixedPoolIndexType background_1_texture_id_;
  FixedPoolIndexType background_2_texture_id_;
  FixedPoolIndexType background_3_texture_id_;
  FixedPoolIndexType background_4_texture_id_;
  FixedPoolIndexType ui_texture_id_;
  FixedPoolIndexType overlay_texture_id_;

  const Vector2 frame_uv_pos_ = {224, 146};
  const Vector2 frame_uv_size_ = {64, 16};

  const Vector2 title_background_uv_pos_ = {0, 112};
  const Vector2 title_background_uv_size_ = {224, 67};

  const Vector2 selected_uv_pos_ = {384, 117};
  const Vector2 selected_uv_size_ = {60, 23};

  const float uv_offset_speed_ = 25.0f;
  float uv_horizontal_offset_ = 0;

  float title_logo_floating_speed_ = 18.0f;
  float movement_acc_ = 0.0f;

  const float selected_frame_moving_speed_ = 2.0f;
  const float selected_frame_moving_range_ = 13.0f;
  uint8_t selected_option_ = 0;

  COLOR fade_overlay_color_ = color::black;
  float fade_overlay_alpha_current_ = 1.0f;
  float fade_overlay_alpha_target_ = 0.0f;

  std::function<void()> fade_overlay_callback_ = {};

  int monster_killed_ = 0;
  float minutes_ = 22;
  float seconds_ = 9;
  int level_ = 0;

  int multiplier_monster_ = 0;
  int multiplier_level_ = 0;
  int multiplier_time_ = 0;
  int score_;

  std::vector<RankingItem> ranking_;

  static inline std::wstring EpochToDateTime(std::int64_t epoch_ms) {
    using namespace std::chrono;
    // epoch_ms -> time_point
    system_clock::time_point tp{milliseconds{epoch_ms}};
    std::time_t tt = system_clock::to_time_t(tp);
    std::tm local_tm{};
    _localtime64_s(&local_tm, &tt);

    std::wstringstream wss;
    wss << std::put_time(&local_tm, L"%Y/%m/%d %H:%M");
    return wss.str();
  }

public:
  ResultUI(GameContext* ctx);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void SetMonsterKilled(int monster_killed) { monster_killed_ = monster_killed; }
  void SetMinutes(float minutes) { minutes_ = minutes; }
  void SetSeconds(float seconds) { seconds_ = seconds; }
  void SetLevel(int level) { level_ = level; }

  void SetMultiplierMonster(int multiplier) { multiplier_monster_ = multiplier; }
  void SetMultiplierLevel(int multiplier) { multiplier_level_ = multiplier; }
  void SetMultiplierTime(int multiplier) { multiplier_time_ = multiplier; }
  void SetScore(int score) { score_ = score; }
  void SetRanking(std::vector<RankingItem> ranking) { ranking_ = ranking; }

  void SetSelectedOption(uint8_t option) {
    selected_option_ = option;
  }

  void SetFadeOverlayAlphaTarget(float alpha, COLOR color, std::function<void()> cb = {}) {
    fade_overlay_alpha_target_ = alpha;
    fade_overlay_color_ = color;

    if (std::fabs(fade_overlay_alpha_target_ - fade_overlay_alpha_current_) < 0.01f) {
      if (cb) cb();
      fade_overlay_callback_ = {};
    }
    else {
      fade_overlay_callback_ = cb;
    }
  }
};
