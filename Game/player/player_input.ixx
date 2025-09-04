module;

export module game.player.input;

import std;

export struct ButtonState {
  bool held = false;
  bool pressed = false;
  bool released = false;
};

export struct PlayerIntent {
  float move_x = 0.0f;
  float move_y = 0.0f;

  // Action
  ButtonState attack;
  ButtonState switch_skill_left;
  ButtonState switch_skill_right;

#if defined(DEBUG) || defined(_DEBUG)
  ButtonState damage_debug;
  ButtonState heal_debug;
  ButtonState exp_debug;
#endif
};

export class IPlayerInput {
public:
  virtual ~IPlayerInput() = default;
  virtual PlayerIntent Intent(float delta_time) = 0;
};
