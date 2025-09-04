module;

export module game.player.input.keyboard;

import std;
import game.player.input;
import game.context;

using Key = KeyCode;

export struct PlayerInputConfig {
  Key up = KeyCode::KK_W;
  Key down = KeyCode::KK_S;
  Key left = KeyCode::KK_A;
  Key right = KeyCode::KK_D;

  Key attack = KeyCode::KK_SPACE;
  Key switch_skill_right = KeyCode::KK_E;
  Key switch_skill_left = KeyCode::KK_Q;
#if defined(DEBUG) || defined(_DEBUG)
  Key damage_debug = KeyCode::KK_F1;
  Key heal_debug = KeyCode::KK_F2;
  Key exp_debug = KeyCode::KK_F5;
#endif
};

export class KeyboardPlayerInput : public IPlayerInput {
public:
  KeyboardPlayerInput(GameContext* ctx, PlayerInputConfig props = {})
    : game_context_(ctx), props(props) {}

  PlayerIntent Intent(float) override {
    PlayerIntent intent;

    const bool up = get(props.up);
    const bool down = get(props.down);
    const bool left = get(props.left);
    const bool right = get(props.right);

    intent.move_x = (right ? 1.f : 0.f) + (left ? -1.f : 0.f);
    intent.move_y = (down ? 1.f : 0.f) + (up ? -1.f : 0.f);

    // handle hypothesis
    float len = std::hypot(intent.move_x, intent.move_y);
    if (len > 1.0f) {
      float inv = 1.0f / len;
      intent.move_x *= inv;
      intent.move_y *= inv;
    }

    // button state
    intent.attack = step_button(props.attack, prev_attack_);
    prev_attack_ = intent.attack.held;

    intent.switch_skill_left = step_button(props.switch_skill_left, prev_skill_switch_left_);
    prev_skill_switch_left_ = intent.switch_skill_left.held;

    intent.switch_skill_right = step_button(props.switch_skill_right, prev_skill_switch_right_);
    prev_skill_switch_right_ = intent.switch_skill_right.held;

#if defined(DEBUG) || defined(_DEBUG)
    intent.damage_debug = step_button(props.damage_debug, prev_damage_);
    intent.heal_debug = step_button(props.heal_debug, prev_heal_);
    intent.exp_debug = step_button(props.exp_debug, prev_exp_);

    prev_damage_ = intent.damage_debug.held;
    prev_heal_ = intent.heal_debug.held;
    prev_exp_ = intent.exp_debug.held;
#endif

    return intent;
  }

private:
  GameContext* game_context_;
  PlayerInputConfig props;

  bool prev_attack_ = false;
  bool prev_skill_switch_left_ = false;
  bool prev_skill_switch_right_ = false;
  bool prev_damage_ = false;
  bool prev_heal_ = false;
  bool prev_exp_ = false;

  bool get(Key k) const {
    return game_context_->input_handler->GetKey(k);
  }

  ButtonState step_button(Key k, bool prevHeld) const {
    const bool now = get(k);
    ButtonState st;
    st.held = now;
    st.pressed = (!prevHeld && now);
    st.released = (prevHeld && !now);
    return st;
  }
};
