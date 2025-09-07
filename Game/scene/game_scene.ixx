module;
#include "stdint.h"

export module game.scene_game;

import std;
import game.scene;
import game.map;
import game.map.map_manager;

import game.player;
import game.scene_object.camera;
import game.scene_game.context;
import game.scene_object.skill;
import game.ui.game_ui;
import game.ui.pause_ui;
import game.ui.level_up;
import game.mobs_manager;
import game.player.level;
import game.ui.status_ui;
import game.utils.throttle;

constexpr int BASE_CHEST_OPEN_EXP = 5;
constexpr int BASE_MONSTER_KILL_EXP = 10;

export class GameScene : public IScene {
private:
  std::unique_ptr<MapManager> map_manager_{nullptr};

  std::unique_ptr<MobManager> mob_manager_ = nullptr;
  std::unique_ptr<SkillManager> skill_manager_ = nullptr;

  std::unique_ptr<Player> player_ = nullptr;
  std::unique_ptr<Camera> camera_ = nullptr;

  // UI
  std::unique_ptr<GameUI> ui_ = nullptr;
  std::unique_ptr<PauseMenuUI> pause_menu_ui_ = nullptr;
  std::unique_ptr<LevelUpUI> level_up_ui_ = nullptr;
  std::unique_ptr<StatusUI> status_ui_ = nullptr;

  std::unique_ptr<SceneContext> scene_context = nullptr;

  bool is_end_ = false;

  int monster_killed_ = 0;
  double timer_elapsed_ = 0; // in seconds

  void HandlePlayerMovementAndCollisions(GameContext* ctx, float delta_time);
  void HandlePlayerEnterMapCollision(float delta_time, SceneContext* scene_ctx);
  void HandleSkillHitMobCollision(GameContext* ctx, float delta_time);
  void HandleMobHitPlayerCollision(GameContext* ctx, float delta_time);
  void HandleSkillHitWallCollision(GameContext* ctx, float delta_time);

  void HandlePauseMenu(GameContext* ctx, float delta_time);
  void HandleLevelUpUI(GameContext* ctx, float delta_time);
  void HandleLevelUpSelection(player_level::OptionType);

  void HandleOnStatusOpen(GameContext* ctx, float delta_time);
  void HandleStatusUpdate(GameContext* ctx, float delta_time);

  void UpdateInput(InputHandler* ih);

  void ResetTimer();

  void UpdateUI(GameContext* ctx, float delta_time);

  // pause menu
  bool is_pause_ = false;
  bool is_allow_pause_ = false;
  uint8_t pause_menu_selected_option_ = 0;

  // level up pause menu
  int player_level_prev_ = 1;
  bool is_show_level_up_ui = false;
  bool is_allow_level_up_ui_control_ = false;
  uint8_t level_up_selected_option_ = 1;
  std::array<player_level::OptionType, 3> level_up_options_ = {};

  // status menu
  bool is_show_status_ui_ = false;
  bool is_allow_status_ui_control_ = false;

  // input
  Throttle ui_throttle{0.2};

  struct GameInput {
    bool is_button_yes = false;
    bool is_button_no = false;

    bool is_pause_menu_button = false;
    bool is_status_menu_button = false;
    bool is_button_up = false;
    bool is_button_down = false;
    bool is_button_left = false;
    bool is_button_right = false;
    bool is_button_left_2 = false;
    bool is_button_right_2 = false;
  } input;

  bool is_x_input_ = false;

public:
  GameScene(bool is_default_x_input = false) { is_x_input_ = is_default_x_input; }
  
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
