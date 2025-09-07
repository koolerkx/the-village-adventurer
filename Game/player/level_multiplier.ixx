module;

export module game.level.level_multiplier;

import std;

export namespace multiplier {
  constexpr float multiplier = 1.07f;

  float GetMobAttackMultiplier(int level) {
    return static_cast<float>(std::pow(multiplier, level));
  }

  float GetMobHPMultiplier(int level) {
    return static_cast<float>(std::pow(multiplier, level));
  }
}
