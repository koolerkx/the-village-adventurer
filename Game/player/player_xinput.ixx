module;

export module game.player.input.xinput;

import std;
import game.player.input;
import game.input;
import game.context;

export struct XInputPlayerInputConfig {
  XButtonCode attack = XButtonCode::A;
  XButtonCode switch_skill_right = XButtonCode::RB;
  XButtonCode switch_skill_left = XButtonCode::LB;
  XButtonCode switch_skill_right_2 = XButtonCode::B;
  XButtonCode switch_skill_left_2 = XButtonCode::Y;

  float deadzone = 0.0f;
  float curve = 0.8f;
  bool invert_y = true;
};

export class XInputPlayerInput final : public IPlayerInput {
public:
  XInputPlayerInput(GameContext* ctx, XInputPlayerInputConfig cfg = {})
    : ctx_(ctx), cfg_(cfg) {}

  PlayerIntent Intent(float) override {
    auto ih = ctx_->input_handler;

    PlayerIntent out{};

    auto [ls, rs] = ih->GetXInputAnalog(); // ls = {x,y}, rs = {x,y}
    float x = applyDeadzone(ls.first);
    float y = applyDeadzone(cfg_.invert_y ? -ls.second : ls.second);

    float len2 = x * x + y * y;
    if (len2 > 1.0f) {
      float invLen = 1.0f / std::sqrt(len2);
      x *= invLen;
      y *= invLen;
    }
    out.move_x = x;
    out.move_y = y;

    out.attack.held = ih->GetXInputButton(cfg_.attack);
    out.attack.pressed = ih->IsXInputButtonDown(cfg_.attack);
    out.attack.released = ih->IsXInputButtonUp(cfg_.attack);

    out.switch_skill_right.held = ih->GetXInputButton(cfg_.switch_skill_right) || ih->GetXInputButton(
      cfg_.switch_skill_right_2);
    out.switch_skill_right.pressed = ih->IsXInputButtonDown(cfg_.switch_skill_right) || ih->IsXInputButtonDown(
      cfg_.switch_skill_right_2);
    out.switch_skill_right.released = ih->IsXInputButtonUp(cfg_.switch_skill_right) || ih->IsXInputButtonUp(
      cfg_.switch_skill_right_2);

    out.switch_skill_left.held = ih->GetXInputButton(cfg_.switch_skill_left) || ih->GetXInputButton(
      cfg_.switch_skill_left_2);
    out.switch_skill_left.pressed = ih->IsXInputButtonDown(cfg_.switch_skill_left) || ih->IsXInputButtonDown(
      cfg_.switch_skill_left_2);
    out.switch_skill_left.released = ih->IsXInputButtonUp(cfg_.switch_skill_left) || ih->IsXInputButtonUp(
      cfg_.switch_skill_left_2);
    return out;
  }

private:
  float applyDeadzone(float v) const {
    float a = std::abs(v);
    if (a <= cfg_.deadzone) return 0.0f;
    float t = (a - cfg_.deadzone) / (1.0f - cfg_.deadzone);
    t = std::pow(t, cfg_.curve);
    return std::copysign(t, v);
  }

  GameContext* ctx_;
  XInputPlayerInputConfig cfg_;
};
