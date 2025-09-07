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
    OptionType::ATTACK, OptionType::DEFENSE, OptionType::MOVING_SPEED, OptionType::HP_UP, OptionType::HEAL
  };

  std::wstring GetOptionDescription(OptionType type) {
    switch (type) {
    case OptionType::ATTACK:
      return L"�U����1.2�{";
    case OptionType::DEFENSE:
      return L"�h���1.2�{";
    case OptionType::MOVING_SPEED:
      return L"�ړ����x1.05�{";
    case OptionType::HP_UP:
      return L"�ő�HP�㏸";
    case OptionType::HEAL:
      return L"HP�S��";
    }
    return L"�G���[";
  };

  std::wstring GetAbilityDescription(Ability type) {
    switch (type) {
    case Ability::ATTACK:
      return L"�U����";
    case Ability::DEFENSE:
      return L"�h���";
    case Ability::MOVING_SPEED:
      return L"�ړ����x";
    case Ability::HP_UP:
      return L"�ő�HP�㏸";
    }
    return L"�G���[";
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
  
  UV GetAbilityUV(Ability type) {
    switch (type) {
    case Ability::ATTACK:
      return {{48, 827}, {16, 16}};
    case Ability::DEFENSE:
      return {{0, 795}, {16, 16}};
    case Ability::MOVING_SPEED:
      return {{0, 811}, {16, 16}};
    case Ability::HP_UP:
      return {{64, 811}, {16, 16}};
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
