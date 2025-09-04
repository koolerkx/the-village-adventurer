#ifndef CI_BUILD
module;

#include  "GameCueSheet_0.h"

export module game.audio.audio_clip;

export enum class audio_clip: int {
  hit_1              = CRI_GAMECUESHEET_0_HIT_1,
  hit_2              = CRI_GAMECUESHEET_0_HIT_2,
  keyboard_click     = CRI_GAMECUESHEET_0_KEYBOARD_CLICK,
  walk_grass         = CRI_GAMECUESHEET_0_WALK_GRASS,
  attack_sword_light = CRI_GAMECUESHEET_0_ATTACK_SWORD_LIGHT,
  attack_sword_mid   = CRI_GAMECUESHEET_0_ATTACK_SWORD_MID,
  ambient_2          = CRI_GAMECUESHEET_0_AMBIENT_2,
  ambient_3          = CRI_GAMECUESHEET_0_AMBIENT_3,
  dark_ambient_1     = CRI_GAMECUESHEET_0_DARK_AMBIENT_1,
  light_ambience_1   = CRI_GAMECUESHEET_0_LIGHT_AMBIENCE_1,
  ambient_1          = CRI_GAMECUESHEET_0_AMBIENT_1,
  blood_2            = CRI_GAMECUESHEET_0_BLOOD2,
  blood_1            = CRI_GAMECUESHEET_0_BLOOD1,
  equip_2            = CRI_GAMECUESHEET_0_EQUIP2,
  equip_1            = CRI_GAMECUESHEET_0_EQUIP1,
  attack_real_sword  = CRI_GAMECUESHEET_0_ATTACK_REAL_SWORD,
  equip_3            = CRI_GAMECUESHEET_0_EQUIP3,
  bgm_base           = CRI_GAMECUESHEET_0_BGM_BASE,
  bgm_title          = CRI_GAMECUESHEET_0_BGM_TITLE,
  bgm_result         = CRI_GAMECUESHEET_0_BGM_RESULT,
  bgm_fight_1        = CRI_GAMECUESHEET_0_BGM_FIGHT_1,
  bgm_fight_2        = CRI_GAMECUESHEET_0_BGM_FIGHT_2,
  skill_fireball     = CRI_GAMECUESHEET_0_SKILL_FIREBALL,
  skill_heal         = CRI_GAMECUESHEET_0_SKILL_HEAL,
  katana5            = CRI_GAMECUESHEET_0_KATANA5,
  katana3            = CRI_GAMECUESHEET_0_KATANA3,
  skill_explosion    = CRI_GAMECUESHEET_0_SKILL_EXPLOSION,
  buff2              = CRI_GAMECUESHEET_0_BUFF2,
  chest_open         = CRI_GAMECUESHEET_0_CHEST_OPEN,
  level_up           = CRI_GAMECUESHEET_0_LEVEL_UP,
  metal_se           = CRI_GAMECUESHEET_0_METAL_SE,
  select_se_1        = CRI_GAMECUESHEET_0_SELECT_SE_1,
  bgm_action_1       = CRI_GAMECUESHEET_0_BGM_ACTION_1,
  bgm_pause_menu     = CRI_GAMECUESHEET_0_BGM_PAUSE_MENU,
  buff1              = CRI_GAMECUESHEET_0_BUFF1,
};
#else

export module game.audio.audio_clip;

export enum class audio_clip: int {
  hit_1 = 0,
  hit_2 = 1,
  keyboard_click = 2,
  walk_grass = 3,
  attack_sword_light = 4,
  attack_sword_mid = 5,
  ambient_2 = 6,
  ambient_3 = 7,
  dark_ambient_1 = 8,
  light_ambience_1 = 9,
  ambient_1 = 10,
  blood_2 = 11,
  blood_1 = 12,
  equip_2 = 13,
  equip_1 = 14,
  attack_real_sword = 15,
  equip_3 = 16,
  bgm_base = 17,
  bgm_title = 18,
  bgm_result = 19,
  bgm_fight_1 = 20,
  bgm_fight_2 = 21,
  skill_fireball = 22,
  skill_heal = 23,
  katana5 = 24,
  katana3 = 25,
  skill_explosion = 26,
  buff2 = 27,
  chest_open = 28,
  level_up = 29,
  metal_se = 30,
  select_se_1 = 31,
  bgm_action_1 = 32,
  bgm_pause_menu = 33,
  buff1 = 34, 
};

#endif
