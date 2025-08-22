module;
#include <chrono>

export module game.utils.throttle;

export class Throttle {
  using Clock = std::chrono::steady_clock;
  Clock::time_point last_call_;
  float interval_seconds_;

public:
  explicit Throttle(float interval_seconds)
    : last_call_(Clock::now() - std::chrono::seconds(1)), // allow at start
      interval_seconds_(interval_seconds) {}

  bool CanCall() {
    auto now = Clock::now();
    float elapsed = std::chrono::duration<float>(now - last_call_).count();
    if (elapsed >= interval_seconds_) {
      last_call_ = now;
      return true;
    }
    return false;
  }

  void Reset() {
    last_call_ = Clock::now();
  }
};
