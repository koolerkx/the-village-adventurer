module;

export module game.ui.interpolation;

import std;

export namespace interpolation {
  enum class SmoothType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut
  };

  float UpdateSmoothValue(float current, float target, float delta_time, SmoothType type, float speed = 5.0f) {
    switch (type) {
    case SmoothType::Linear: {
      float step = speed * delta_time;
      if (current < target)
        return std::min(current + step, target);
      else
        return std::max(current - step, target);
    }
    case SmoothType::EaseIn: {
      float t = std::clamp(delta_time * speed, 0.0f, 1.0f);
      float eased = t * t; // quadratic ease in
      return std::lerp(current, target, eased);
    }
    case SmoothType::EaseOut: {
      float t = std::clamp(delta_time * speed, 0.0f, 1.0f);
      float eased = 1 - (1 - t) * (1 - t); // quadratic ease out
      return std::lerp(current, target, eased);
    }
    case SmoothType::EaseInOut: {
      float t = std::clamp(delta_time * speed, 0.0f, 1.0f);
      float eased = (t < 0.5f)
                      ? 2 * t * t
                      : 1 - static_cast<float>(std::pow(-2 * t + 2, 2)) / 2;
      return std::lerp(current, target, eased);
    }
    }
    return target;
  }
}
