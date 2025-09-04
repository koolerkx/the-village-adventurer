module;

export module game.field_object.chest;

import std;
import graphic.utils.color;

export namespace chest {
  enum class RewardType: unsigned char {
    BUFF_ATTACK_POWER,
    // BUFF_ATTACK_SPEED,
    BUFF_MOVING_SPEED,
    HEAL,
    INVINCIBLE
  };

  RewardType GetRandomRewardType();

  std::wstring GetChestRewardEventText(RewardType t);
  COLOR GetChestRewardEventColor(RewardType t);
  std::wstring GetChestLogText(RewardType t);
}