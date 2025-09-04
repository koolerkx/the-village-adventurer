module;

export module game.ui.status_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;
import game.types;
import game.player.level;
import game.player.buff;

struct StatusUIActiveProps {
  float hp;
  float max_hp;
  float defense;
  float attack;
  float speed;
  float experience;
  float max_experience;
  float total_experience;
  float level;
  std::vector<player_level::PlayerAbility> abilities;
  std::vector<PlayerBuff> buffs;
};

export class StatusUI {
private:
  std::wstring font_key_;
  Font* default_font_;

  FixedPoolIndexType background_texture_id_;
  FixedPoolIndexType ui_texture_id_;

  // background
  const Vector2 overlay_uv_pos_ = {96, 297};
  const Vector2 overlay_uv_size_ = {8, 8};
  const Vector2 background_uv_pos_ = {0, 875};
  const Vector2 background_uv_size_ = {840, 480};

  // Status bar frame
  const Vector2 status_bar_frame_uv_pos_ = {384, 0};
  const Vector2 status_bar_frame_uv_size_ = {160, 41};
  const Vector2 circle_uv_pos_ = {384, 41};
  const Vector2 circle_uv_size_ = {41, 41};
  const Vector2 hp_bar_uv_pos_ = {224, 128};
  const Vector2 hp_bar_uv_size_ = {120, 8};
  const Vector2 hp_bar_frame_uv_pos_ = {224, 112};
  const Vector2 hp_bar_frame_uv_size_ = {128, 16};

  // button
  const Vector2 frame_uv_pos_ = {224, 146};
  const Vector2 frame_uv_size_ = {64, 16};
  const Vector2 selected_uv_pos_ = {384, 117};
  const Vector2 selected_uv_size_ = {60, 23};

  const float selected_frame_moving_speed_ = 2.0f;
  const float selected_frame_moving_range_ = 13.0f;

  float movement_acc_ = 0.0f;

  float opacity_ = 1.0f;

  StatusUIActiveProps props_ = {
    .hp = 80,
    .max_hp = 100,
    .defense = 100,
    .attack = 100,
    .speed = 100,
    .experience = 100,
    .max_experience = 100,
    .total_experience = 100,
    .level = 99,
    .abilities = {
      player_level::PlayerAbility{
        .type = player_level::Ability::ATTACK,
        .multiplier = 1.2,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::DEFENSE,
        .multiplier = 1.2,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::MOVING_SPEED,
        .multiplier = 1.2,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::HP_UP,
        .value = 10,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::ATTACK,
        .multiplier = 1.2,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::DEFENSE,
        .multiplier = 1.2,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::MOVING_SPEED,
        .multiplier = 1.2,
      },
      player_level::PlayerAbility{
        .type = player_level::Ability::HP_UP,
        .value = 10,
      },
    },
    .buffs = {
      PlayerBuff{.type = BuffType::ATTACK_POWER},
      PlayerBuff{.type = BuffType::ATTACK_POWER},
      PlayerBuff{.type = BuffType::INVINCIBLE},
      PlayerBuff{.type = BuffType::MOVING_SPEED},
      PlayerBuff{.type = BuffType::MOVING_SPEED},
    },
  };

public:
  StatusUI(GameContext* ctx);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void Reset() {
    movement_acc_ = 0.0f;
  }
};
