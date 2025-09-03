module;

module game.field_object.chest;

import std;
import game.player.buff;
import game.utils.helper;

namespace chest {
  const std::vector<RewardType> rewards{
    RewardType::BUFF_ATTACK_POWER,
    // RewardType::BUFF_ATTACK_SPEED,
    RewardType::BUFF_MOVING_SPEED,
    RewardType::HEAL,
    RewardType::INVINCIBLE,
  };

  RewardType GetRandomRewardType() {
    return helper::GetRandomElement(rewards);
  }
}
