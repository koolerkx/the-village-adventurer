module;

export module game.ui.game_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;

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
  {"RoundBackground", UV{{223, 146}, {64, 16}}},
  {"KeyboardSpaceUp", UV{{96, 563}, {32, 16}}},
  {"KeyboardSpaceDown", UV{{96, 627}, {32, 16}}},

  // Skill
  {"SkillSlot", UV{{384, 82}, {34, 34}}},
  {"SkillSelected", UV{{384, 140}, {26, 26}}},

  // Coin
  {"CoinWithoutStroke", UV{{476, 41}, {16, 18}}},
  {"Coin", UV{{492, 41}, {16, 18}}},
};

export class GameUI {
private:
  FixedPoolIndexType texture_id_;
  std::wstring font_key_;

  std::vector<std::wstring> text_list_ = {
    L"プレイヤーが200ダメージ受けた",
    L"プレイヤーが斬撃を出した",
    L"スライムが200ダメージ受けた",
    L"プレイヤーが200HP回復した",
    L"プレイヤーが200ゴールドもらった",
  };
  std::wstring timer_text_ = L"12:34";
  Font* default_font_;

  int skill_count_ = 3;
  int skill_selected_ = 0;

  int coin_text_ = 987;

public:
  GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path);
  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
};
