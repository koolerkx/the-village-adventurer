module;

export module game.ui.game_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;
import game.types;
import game.scene_object.skill;
import game.player.buff;

std::unordered_map<std::string, UV> texture_map = {
  // HP Bar
  {"HPBarPanel", UV{{384, 0}, {160, 41}}},
  {"BigCircle", UV{{384, 41}, {41, 41}}},
  {"Heart", UV{{508, 41}, {32, 32}}},
  {"HPBar", UV{{224, 128}, {120, 8}}},
  {"HPBarFrame", UV{{224, 112}, {128, 16}}},

  // Event Log
  {"Block", UV{{96, 297}, {8, 8}}},
  {"TimerBackground", UV{{215, 272}, {119, 17}}},

  // Attack Hint
  {"RoundBackground", UV{{224, 146}, {64, 16}}},
  {"KeyboardSpaceUp", UV{{64, 579}, {32, 16}}},
  {"KeyboardSpaceDown", UV{{64, 643}, {32, 16}}},

  // Skill
  {"SkillSlot", UV{{384, 82}, {34, 34}}},
  {"SkillSelected", UV{{384, 140}, {26, 26}}},
  {"KeyboardQ", UV{{0, 387}, {16, 16}}},
  {"KeyboardE", UV{{64, 355}, {16, 16}}},

  // Coin
  {"CoinWithoutStroke", UV{{476, 41}, {16, 18}}},
  {"Coin", UV{{492, 41}, {16, 18}}},

  // Input Hint
  {"Corner", UV{{129, 242}, {33, 15}}},
  {"KeyboardWDown", UV{{96, 499}, {16, 16}}},
  {"KeyboardADown", UV{{0, 467}, {16, 16}}},
  {"KeyboardSDown", UV{{32, 499}, {16, 16}}},
  {"KeyboardDDown", UV{{48, 467}, {16, 16}}},
  {"KeyboardWUp", UV{{96, 387}, {16, 16}}},
  {"KeyboardAUp", UV{{0, 355}, {16, 16}}},
  {"KeyboardSUp", UV{{32, 387}, {16, 16}}},
  {"KeyboardDUp", UV{{48, 355}, {16, 16}}},

  // Area Message
  {"MessageUpper", UV{{96, 289}, {83, 8}}},
  {"MessageLower", UV{{179, 289}, {83, 8}}},

  // full screen overlay
  {"DamageOverlay", UV{{128, 323}, {320, 180}}},
  {"HealOverlay", UV{{128, 503}, {320, 180}}},

  // Experience Coin
  {"Star", UV{{452, 106}, {32, 32}}},
  {"StarAdditive", UV{{452, 138}, {32, 32}}},
};

struct DamageTextProps {
  POSITION position;
  std::wstring skill_name;
  short damage;
  float opacity = 1;
};

struct EventTextProps {
  POSITION position;
  std::wstring text;
  COLOR color;
  float opacity = 1;
};

struct EventLogText {
  std::wstring text;
  COLOR color;
};

struct ExperienceStar {
  POSITION position;
  int value;
  std::function<void(int)> callback;
  float floating_timeout = 1.0f;
};

struct StarTrajectory {
  POSITION position;
  float size = 24.0f;
};

export class GameUI {
private:
  FixedPoolIndexType texture_id_;
  FixedPoolIndexType fade_overlay_texture_id_;
  std::wstring font_key_;

  std::deque<EventLogText> text_list_;
  std::wstring timer_text_ = L"99:99";
  Font* default_font_;

  int skill_count_ = 3;
  int skill_selected_ = 0;
  std::vector<UV> skill_uvs_ = {};

  float hp_percentage_target_ = 1.0f;
  float hp_percentage_current_ = 1.0f;

  int coin_text_ = 987;

  bool is_get_damage_frame_ = false;
  bool is_hp_flashing_ = false;
  float heal_flash_opacity_target_ = 0.0f;
  float heal_flash_opacity_current_ = 0.0f;
  float damage_flash_opacity_target_ = 0.0f;
  float damage_flash_opacity_current_ = 0.0f;

  bool is_showing_area_message_ = false;
  std::wstring area_message_;
  float area_message_opacity_target_ = 0.0f;
  float area_message_opacity_current_ = 0.0f;

  float fade_overlay_alpha_target_ = 1.0f;
  float fade_overlay_alpha_current_ = 1.0f;
  COLOR fade_overlay_color_ = color::black;
  std::function<void()> fade_overlay_callback_ = {};

  float ui_opacity_target_ = 0.0f;
  float ui_opacity_current_ = 0.0f;

  bool is_show_ui_ = true;

  // constant flags
  const bool is_show_skill_ = true;
  const bool is_show_coin_ = false;
  const bool is_show_event_log_ = true;

  std::vector<DamageTextProps> damage_texts;

  std::vector<EventTextProps> event_texts = {};

  std::vector<PlayerBuff> player_buffs_ = {};

  std::vector<ExperienceStar> experience_stars_ = {};
  std::vector<StarTrajectory> experience_stars_trajectory_ = {};
  const Vector2 EXP_COIN_TARGET_POS = {64, 56};

public:
  void SetHpPercentage(float percentage) {
    if (std::abs(hp_percentage_target_ - percentage) > 0.00001f) {
      is_get_damage_frame_ = true;

      if (hp_percentage_target_ > percentage)
        damage_flash_opacity_current_ = 1;
      else heal_flash_opacity_current_ = 1;
    };
    hp_percentage_target_ = percentage;
  }

  void SetTimerText(double elapsed_seconds) {
    int total_seconds = static_cast<int>(elapsed_seconds);

    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;

    std::wstringstream wss;
    wss << std::setw(2) << std::setfill(L'0') << minutes
      << L":"
      << std::setw(2) << std::setfill(L'0') << seconds;
    timer_text_ = wss.str();
  }

  void AddLogText(std::wstring text, COLOR color) {
    text_list_.emplace_back(EventLogText{
      .text = text,
      .color = color
    });
  }

  void SetSkillSelected(int i) {
    skill_selected_ = i % skill_count_;
  }

  void SetIsShowUI(bool is_show_ui) {
    is_show_ui_ = is_show_ui;
  }

  // position is the center of object
  void AddDamageText(Vector2 position, std::wstring name, short damage) {
    damage_texts.emplace_back(DamageTextProps{
      {position.x, position.y, 0}, name, damage
    });
  }

  void PlayEnterAreaMessage(std::wstring);

  GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path);

  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time, Camera* camera);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
  void RenderDamageText(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
  void RenderExperienceCoin(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);

  void SetFadeOverlayAlphaTarget(float alpha, COLOR color, std::function<void()> cb = {}) {
    fade_overlay_alpha_target_ = alpha;
    fade_overlay_color_ = color;

    if (std::fabs(fade_overlay_alpha_target_ - fade_overlay_alpha_current_) <= 0.01f) {
      if (cb) cb();
      fade_overlay_callback_ = {};
    }
    else {
      fade_overlay_callback_ = cb;
    }
  }

  void InitSkillData(const std::vector<SKILL_TYPE> data) {
    skill_count_ = static_cast<int>(data.size());
    skill_selected_ = 0;
    skill_uvs_.clear();

    for (auto d : data) {
      auto it = skill_data.find(d);
      if (it != skill_data.end()) {
        skill_uvs_.push_back(it->second.icon_uv);
      }
    }
  }

  void SetUIOpacity(float alpha) {
    ui_opacity_target_ = alpha;
  }

  void UpdatePlayerBuffs(const std::vector<PlayerBuff>& pb) {
    player_buffs_ = pb;
  }

  void AddEventText(const std::wstring& text, const COLOR& color) {
    event_texts.emplace_back(EventTextProps{
      .text = text,
      .color = color
    });
  }
  
  void AddExperienceCoin(Vector2 position, int value, const std::function<void(int)>& callback) {
    experience_stars_.emplace_back(ExperienceStar{
      {position.x, position.y, 0}, value, callback
    });
  }
};
