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
    HP_UP
  };

  std::vector<OptionType> option_list = {
    OptionType::ATTACK, OptionType::DEFENSE, OptionType::MOVING_SPEED, OptionType::HP_UP
  };

  std::wstring GetOptionDescription(OptionType type) {
    switch (type) {
    case OptionType::ATTACK:
      return L"�U����UP";
    case OptionType::DEFENSE:
      return L"�h���UP";
    case OptionType::MOVING_SPEED:
      return L"�ړ����xUP";
    case OptionType::HP_UP:
      return L"HP��";
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
      return {{0, 779}, {16, 16}};
    }
    return {};
  }
}
