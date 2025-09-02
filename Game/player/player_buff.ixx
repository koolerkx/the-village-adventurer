module;
#include "stdint.h"

export module game.player.buff;

import std;
import graphic.utils.types;

export enum class BuffType: uint8_t {
  ATTACK_POWER,
  ATTACK_SPEED,
  MOVING_SPEED
};

export struct PlayerBuff {
  float duration = 10.0f;
  float elapsed = 0.0f;
  float multiplier = 1.5f;
  BuffType type;
};

export void UpdateActiveBuffs(std::vector<PlayerBuff>& buffs, const float dt) {
  for (auto& b : buffs) {
    b.elapsed += dt;
  }

  std::erase_if(buffs, [](const PlayerBuff b) { return b.elapsed > b.duration; });
}

export float GetBuffMultiplier(std::vector<PlayerBuff> buffs, BuffType type) {
  float multiplier = 1.0f;
  for (auto b : buffs) {
    if (b.type != type) continue;
    multiplier *= b.multiplier;
  }
  return multiplier;
}

export UV GetBuffIconUV(BuffType t) {
  switch (t) {
  case BuffType::ATTACK_POWER:
    return {{48, 827}, {16, 16}};
  case BuffType::ATTACK_SPEED:
    return {{32, 827}, {16, 16}};
  case BuffType::MOVING_SPEED:
    return {{0, 811}, {16, 16}};
  }
  return {};
}

export std::wstring GetBuffDisplayText(BuffType t) {
  switch (t) {
  case BuffType::ATTACK_POWER:
    return L"çUåÇóÕUPÅ@";
  case BuffType::ATTACK_SPEED:
    return L"çUåÇë¨ìxUP";
  case BuffType::MOVING_SPEED:
    return L"à⁄ìÆë¨ìxUP";
  }
  return {};
}
