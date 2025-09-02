module;

export module game.field_object.chest;

import std;

export namespace chest {
  enum class RewardType: unsigned char {
    BUFF_ATTACK_POWER,
    BUFF_ATTACK_SPEED,
    BUFF_MOVING_SPEED,
    HEAL
  };

  RewardType GetRandomRewardType();
}
