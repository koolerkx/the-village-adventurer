module;

module game.field_object.chest;

import std;
import game.player.buff;
import game.utils.helper;
import graphic.utils.color;

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

  std::wstring GetChestRewardEventText(RewardType t)
  {
      switch (t) {
      case RewardType::BUFF_ATTACK_POWER:
        return L"宝箱：攻撃力UP";
      case RewardType::BUFF_MOVING_SPEED:
        return L"宝箱：移動速度UP";
      case RewardType::HEAL:
        return L"宝箱：HP回復１０％";
      case RewardType::INVINCIBLE:
        return L"宝箱：無敵状態";
      }
      return {};
  }

  COLOR GetChestRewardEventColor(RewardType t)
  {
    switch (t) {
    case RewardType::BUFF_ATTACK_POWER:
      return color::redA400;
    case RewardType::BUFF_MOVING_SPEED:
      return color::yellowA400;
    case RewardType::HEAL:
      return color::lightGreenA400;
    case RewardType::INVINCIBLE:
      return color::purpleA400;
    }
    return {};
  }
  
  std::wstring GetChestLogText(RewardType t)
  {
    switch (t) {
    case RewardType::BUFF_ATTACK_POWER:
      return L"宝箱を開けた！『攻撃力UP』を手に入れた！";
    case RewardType::BUFF_MOVING_SPEED:
      return L"宝箱を開けた！『移動速度UP』を手に入れた！";
    case RewardType::HEAL:
      return L"宝箱を開けた！HPが10%回復した！";
    case RewardType::INVINCIBLE:
      return L"宝箱を開けた！一定時間『無敵状態』になった！";
    }
    return {};
  }
}
