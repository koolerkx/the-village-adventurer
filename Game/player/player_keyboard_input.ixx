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

  Key up_2 = KeyCode::KK_UP;
  Key down_2 = KeyCode::KK_DOWN;
  Key left_2 = KeyCode::KK_LEFT;
  Key right_2 = KeyCode::KK_RIGHT;

  Key attack = KeyCode::KK_SPACE;
  Key attack_2 = KeyCode::KK_ENTER;
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
    const bool up_2 = get(props.up_2);
    const bool down = get(props.down);
    const bool down_2 = get(props.down_2);
    const bool left = get(props.left);
    const bool left_2 = get(props.left_2);
    const bool right = get(props.right);
    const bool right_2 = get(props.right_2);

    intent.move_x = (right || right_2 ? 1.f : 0.f) + (left || left_2 ? -1.f : 0.f);
    intent.move_y = (down || down_2 ? 1.f : 0.f) + (up || up_2 ? -1.f : 0.f);

    // handle hypothesis
    float len = std::hypot(intent.move_x, intent.move_y);
    if (len > 1.0f) {
      float inv = 1.0f / len;
      intent.move_x *= inv;
      intent.move_y *= inv;
    }

    // button state
    intent.attack.held = get(props.attack) || get(props.attack_2);
    intent.attack.pressed = GetKeyDown(props.attack) || GetKeyDown(props.attack_2);
    intent.attack.released = GetKeyUp(props.attack) || GetKeyUp(props.attack_2);

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

  bool prev_skill_switch_left_ = false;
  bool prev_skill_switch_right_ = false;
  bool prev_damage_ = false;
  bool prev_heal_ = false;
  bool prev_exp_ = false;

  bool get(Key k) const {
    return game_context_->input_handler->GetKey(k);
  }

  bool GetKeyDown(Key k) const {
    return game_context_->input_handler->IsKeyDown(k);
  }

  bool GetKeyUp(Key k) const {
    return game_context_->input_handler->IsKeyUp(k);
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
