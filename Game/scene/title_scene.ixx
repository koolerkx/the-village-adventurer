module;
#include "stdint.h"

export module game.title_scene;

import std;
import game.scene;
import game.ui.title_ui;
import game.utils.throttle;

enum class SelectedOption: uint8_t {
  START_GAME,
  END_GAME
};

export class TitleScene : public IScene {
private:
  std::unique_ptr<TitleUI> title_ui_{nullptr};

  const int options_count = 2;
  uint8_t selected_option_ = 0;

  Throttle input_throttle_{0.3f};
  Throttle enter_throttle_{0.2f};
  bool is_allow_control_ = true;

  bool is_x_input_ = false;
public:
  TitleScene(bool is_default_x_input = false);

  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
