module;

export module game.player.input.composite;

import std;
import game.player.input;

export class CompositePlayerInput final : public IPlayerInput {
public:
  CompositePlayerInput(std::unique_ptr<IPlayerInput> primary,
                       std::unique_ptr<IPlayerInput> secondary,
                       bool normalize_diagonal = true)
    : a_(std::move(primary)), b_(std::move(secondary)), normalize_(normalize_diagonal) {}

  PlayerIntent Intent(float dt) override {
    PlayerIntent pa = a_ ? a_->Intent(dt) : PlayerIntent{};
    PlayerIntent pb = b_ ? b_->Intent(dt) : PlayerIntent{};

    float ax2 = pa.move_x * pa.move_x + pa.move_y * pa.move_y;
    if (ax2 > 0) {
      if (normalize_ && ax2 > 1.0f) {
        float inv = 1.0f / std::sqrt(ax2);
        pa.move_x *= inv;
        pa.move_y *= inv;
      }
    }
    else {
      float bx2 = pb.move_x * pb.move_x + pb.move_y * pb.move_y;
      if (normalize_ && bx2 > 1.0f) {
        float inv = 1.0f / std::sqrt(bx2);
        pb.move_x *= inv;
        pb.move_y *= inv;
      }
      pa.move_x = pb.move_x;
      pa.move_y = pb.move_y;
    }

    pa.attack = merge(pa.attack, pb.attack);
    pa.switch_skill_left = merge(pa.switch_skill_left, pb.switch_skill_left);
    pa.switch_skill_right = merge(pa.switch_skill_right, pb.switch_skill_right);
#if defined(DEBUG) || defined(_DEBUG)
    pa.damage_debug = merge(pa.damage_debug, pb.damage_debug);
    pa.heal_debug = merge(pa.heal_debug, pb.heal_debug);
    pa.exp_debug = merge(pa.exp_debug, pb.exp_debug);
#endif
    return pa;
  }

private:
  static ButtonState merge(const ButtonState& A, const ButtonState& B) {
    return ButtonState{
      .held = A.held || B.held,
      .pressed = A.pressed || B.pressed,
      .released = A.released || B.released
    };
  }

  std::unique_ptr<IPlayerInput> a_;
  std::unique_ptr<IPlayerInput> b_;
  bool normalize_;
};
