module;
#include "stdint.h"

export module game.player.level;

import std;
import graphic.utils.types;

export namespace player_level {
  enum class OptionType: uint8_t {
    ATTACK,
    DEFENSE,
    MOVING_SPEED,
    HP_UP,
    HEAL
  };

  enum class Ability: uint8_t {
    ATTACK,
    DEFENSE,
    MOVING_SPEED,
    HP_UP,
  };


  struct PlayerAbility {
    Ability type;
    float multiplier = 1.0f;
    float value = 0;
  };

  std::vector<OptionType> option_list = {
    OptionType::ATTACK, OptionType::DEFENSE, OptionType::MOVING_SPEED, OptionType::HEAL
  };

  std::wstring GetOptionDescription(OptionType type) {
    switch (type) {
    case OptionType::ATTACK:
      return L"çUåÇóÕUP";
    case OptionType::DEFENSE:
      return L"ñhå‰óÕUP";
    case OptionType::MOVING_SPEED:
      return L"à⁄ìÆë¨ìxUP";
    case OptionType::HP_UP:
      return L"ç≈ëÂHPè„è∏";
    case OptionType::HEAL:
      return L"HPâÒïú";
    }
    return L"ÉGÉâÅ[";
  };

  UV GetOptionUV(OptionType type) {
    switch (type) {
    case OptionType::ATTACK:
      return {{48, 827}, {16, 16}};
    case OptionType::DEFENSE:
      return {{0, 795}, {16, 16}};
    case OptionType::MOVING_SPEED:
      return {{0, 811}, {16, 16}};
    case OptionType::HP_UP:
      return {{64, 811}, {16, 16}};
    case OptionType::HEAL:
      return {{0, 779}, {16, 16}};
    }
    return {};
  }

  float GetLevelAbilityMultiplier(std::vector<PlayerAbility> abilities, Ability type) {
    float multiplier = 1.0f;
    for (auto a : abilities) {
      if (a.type != type) continue;
      multiplier *= a.multiplier;
    }
    return multiplier;
  }

  float GetLevelAbilityValue(std::vector<PlayerAbility> abilities, Ability type) {
    float value = 0.0f;
    for (auto a : abilities) {
      if (a.type != type) continue;
      value += a.value;
    }
    return value;
  }
}
