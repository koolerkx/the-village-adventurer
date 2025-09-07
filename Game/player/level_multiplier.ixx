module;

export module game.level.level_multiplier;

import std;

export namespace multiplier {
  constexpr float attack_multiplier = 1.05f;
  constexpr float hp_multiplier = 1.07f;

  float GetMobAttackMultiplier(int level) {
    return static_cast<float>(std::pow(attack_multiplier, level));
  }

  float GetMobHPMultiplier(int level) {
    return static_cast<float>(std::pow(hp_multiplier, level));
  }
}
