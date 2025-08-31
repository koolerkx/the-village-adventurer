module;

export module game.mobs.slime_green;
import std;
import game.mobs_manager;
import game.map.tilemap_object_handler;
import game.collision.collider;
import graphic.utils.types;
import game.scene_object;
import game.types;
import game.math;

export namespace mob {
  namespace slime_green {
    std::unordered_map<MobActionState, scene_object::AnimationFrameData> animation_data{
#pragma region SLIME_ANIMATION_DATA
      {
        MobActionState::IDLE_DOWN, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 512, 896),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::IDLE_UP, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 512, 960),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::IDLE_LEFT, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 512, 1024),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::IDLE_RIGHT, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 512, 1088),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::ATTACK_DOWN, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 32),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::ATTACK_UP, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 96),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::ATTACK_LEFT, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 160),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::ATTACK_RIGHT, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 224),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::HURT_DOWN, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 512, 608),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::HURT_UP, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 512, 672),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::HURT_LEFT, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 512, 736),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::HURT_RIGHT, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 512, 800),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::DEATH_DOWN, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 320),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::DEATH_UP, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 384),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::DEATH_LEFT, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 448),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::DEATH_RIGHT, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 512, 512),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::MOVING_DOWN, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 512, 1472),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
      {
        MobActionState::MOVING_UP, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 512, 1536),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
      {
        MobActionState::MOVING_LEFT, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 512, 1600),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
      {
        MobActionState::MOVING_RIGHT, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 512, 1664),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
#pragma endregion
    };

    MobState MakeMob(TileMapObjectProps props) {
      constexpr int DEFAULT_HP = 20;

      const Transform t = {
        .position = {props.x, props.y, 0.0},
        .size = {32, 32},
        .scale = {1.0f, 1.0f},
        .rotation_radian = 0,
        .rotation_pivot = {0, 0, 0},
        .position_anchor = {0, 0, 0},
      };

      const UV uv = {
        {512, 896},
        {64, 64}
      };

      constexpr float COLLIDER_PADDING = 6.0f;
      const Collider<MobState> c = {
        .is_trigger = true,
        .position = {props.x + t.size.x / 2, props.y + t.size.x / 2},
        .owner = nullptr, // handle outside
        .shape = CircleCollider{
          .x = 0, .y = 0, .radius = 16 - COLLIDER_PADDING * 2
        }
      };

      const Collider<MobState> atk_c = {
        .is_trigger = true,
        .position = {props.x + t.size.x / 2, props.y + t.size.x / 2},
        .owner = nullptr, // handle outside
        .shape = CircleCollider{
          .x = 0, .y = 0, .radius = 12
        }
      };

      return {
        .transform = t,
        .uv = uv,
        .collider = c,
        .attack_range_collider = atk_c,
        .type = MobType::SLIME_GREEN,
        .state = MobActionState::IDLE_DOWN,
        .inactive_position = {props.x + t.size.x / 2, props.y + t.size.x / 2},
        .is_battle = false, // TODO: change the flag trigger by active area
        .is_loop = true,
        .is_playing = true,
        .hp = DEFAULT_HP
      };
    };

    RenderInstanceItem GetRenderInstanceItem(MobState state) {
      return RenderInstanceItem{
        .transform = state.transform,
        .uv = {
          {state.uv.position.x, state.uv.position.y},
          {state.uv.size.x, state.uv.size.y},
        },
        .color = color::white,
      };
    }

    void HandleDeath(MobState& state) {
      state.state = MobActionState::DEATH_DOWN; // todo: handle facing direction
      state.current_frame = 0;
      state.is_loop = false;
      state.is_playing = true;
      state.current_frame_time = animation_data[MobActionState::DEATH_DOWN].frame_durations[0];
    }

    void HandleHurt(MobState& state) {
      if (is_attack_state(state.state)) return;
      state.state = MobActionState::HURT_DOWN; // todo: handle facing direction
      state.current_frame = 0;
      state.is_loop = false;
      state.is_playing = true;
      state.current_frame_time = animation_data[MobActionState::HURT_DOWN].frame_durations[0];
    }

    void HandleMovement(MobState& state, Vector2 destination) {
      constexpr float SPEED = 25.0f; // todo: extract as config
      state.velocity.x = 0;

      Vector2 mob_center = {
        state.transform.position.x + state.transform.size.x / 2, state.transform.position.y + state.transform.size.y / 2
      };
      Vector2 dir = math::GetDirection(mob_center, destination);

      // Move in easing function
      float animation_progress = static_cast<float>(state.current_frame) / static_cast<float>(animation_data[state.
        state].frames.size());

      float speed = SPEED * math::interpolation::EaseInOutQuint(animation_progress);
      state.velocity.x = dir.x * speed;
      state.velocity.y = dir.y * speed;
    }

    void SyncCollider(MobState& state) {
      state.collider.position = {
        state.transform.position.x + state.transform.size.x / 2,
        state.transform.position.y + state.transform.size.y / 2,
      };
      state.attack_range_collider.position = {
        state.transform.position.x + state.transform.size.x / 2,
        state.transform.position.y + state.transform.size.y / 2,
      };
    }

    MobHitBox GetHitBox(MobState state) {
      return MobHitBox{
        .transform = {
          {
            state.transform.position.x + state.transform.size.x / 2,
            state.transform.position.y + state.transform.size.y / 2, 0
          }
        },
        .collider = Collider<MobHitBox>{
          .is_trigger = true,
          .position = {
            state.transform.position.x + state.transform.size.x / 2,
            state.transform.position.y + state.transform.size.y / 2
          },
          .owner = nullptr, // handle outside
          .shape = CircleCollider{
            .x = 0, .y = 0, .radius = 8
          }
        },
        .damage = 10,
        .attack_delay = 0.5f,
        .timeout = 0.8f,
        .is_animated = false,
        .hit_player = false,
      };
    }

    void UpdateMob(MobState& mob_state, float delta_time, Vector2 player_position) {
      // handle animation end
      if (!mob_state.is_playing) {
        if (is_hurt_state(mob_state.state)    // when hurt animation end
          || is_attack_state(mob_state.state) // when attack animation end
          || is_moving_state(mob_state.state) // when moving animation end
        ) {
          mob_state.state = MobActionState::IDLE_DOWN;
          mob_state.is_loop = true;
          mob_state.is_playing = true;
          mob_state.current_frame = 0;
          mob_state.current_frame_time = 0;
        }
        if (is_death_state(mob_state.state)) {
          mob_state.is_alive = false;
        }
      }

      // Change the state to moving if needed
      mob_state.moving_cooldown = mob_state.moving_cooldown >= 0.0f ? mob_state.moving_cooldown - delta_time : -1;
      if (is_idle_state(mob_state.state) && mob_state.moving_cooldown < 0.0f) {
        constexpr float MOVING_COOLDOWN = 2.0;
        mob_state.moving_cooldown += MOVING_COOLDOWN;
        if (mob_state.is_battle) {
          mob_state.state = MobActionState::MOVING_DOWN;
          mob_state.is_loop = false;
          mob_state.is_playing = true;
          mob_state.current_frame = 0;
          mob_state.current_frame_time = 0;
        }
        else if (is_idle_state(mob_state.state) && !mob_state.is_battle && !is_position_reset(
          {
            mob_state.transform.position.x + mob_state.transform.size.x / 2,
            mob_state.transform.position.y + mob_state.transform.size.y / 2
          }, mob_state.inactive_position)) {
          mob_state.state = MobActionState::MOVING_DOWN;
          mob_state.is_loop = false;
          mob_state.is_playing = true;
          mob_state.current_frame = 0;
          mob_state.current_frame_time = 0;
        }
      }
      if (is_moving_state(mob_state.state)) {
        HandleMovement(mob_state, mob_state.is_battle ? player_position : mob_state.inactive_position);
      }

      // update animation
      scene_object::AnimationState anim_state{
        .is_loop = mob_state.is_loop,
        .play_on_start = true,
        .is_playing = mob_state.is_playing,
        .frames = animation_data[mob_state.state].frames,
        .frame_durations = animation_data[mob_state.state].frame_durations,
        .current_frame = mob_state.current_frame,
        .current_frame_time = mob_state.current_frame_time
      };

      scene_object::UpdateAnimation(anim_state, delta_time, mob_state.uv);

      mob_state.current_frame = anim_state.current_frame;
      mob_state.current_frame_time = anim_state.current_frame_time;
      mob_state.is_playing = anim_state.is_playing;
    }
  }
}
