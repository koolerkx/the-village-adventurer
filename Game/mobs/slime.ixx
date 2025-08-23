module;

export module game.mobs.slime;
import std;
import game.mobs_manager;
import game.map.tilemap_object_handler;
import game.collision.collider;
import graphic.utils.types;
import game.scene_object;

export namespace mob {
  namespace slime {
    std::unordered_map<MobActionState, scene_object::AnimationFrameData> animation_data{
#pragma region SLIME_ANIMATION_DATA
      {
        MobActionState::IDLE_DOWN, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 1152, 896),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::IDLE_UP, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 1152, 960),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::IDLE_LEFT, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 1152, 1024),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::IDLE_RIGHT, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 1152, 1088),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      },
      {
        MobActionState::ATTACK_DOWN, {
          .frames = scene_object::MakeFramesVector(11, 64, 64, 11, 1152, 32),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 11),
        }
      },
      {
        MobActionState::ATTACK_UP, {
          .frames = scene_object::MakeFramesVector(11, 64, 64, 11, 1152, 96),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 11),
        }
      },
      {
        MobActionState::ATTACK_LEFT, {
          .frames = scene_object::MakeFramesVector(11, 64, 64, 11, 1152, 160),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 11),
        }
      },
      {
        MobActionState::ATTACK_RIGHT, {
          .frames = scene_object::MakeFramesVector(11, 64, 64, 11, 1152, 224),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 11),
        }
      },
      {
        MobActionState::HURT_DOWN, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 1152, 608),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::HURT_UP, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 1152, 672),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::HURT_LEFT, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 1152, 736),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::HURT_RIGHT, {
          .frames = scene_object::MakeFramesVector(5, 64, 64, 5, 1152, 800),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 5),
        }
      },
      {
        MobActionState::DEATH_DOWN, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 1152, 320),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::DEATH_UP, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 1152, 384),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::DEATH_LEFT, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 1152, 448),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::DEATH_RIGHT, {
          .frames = scene_object::MakeFramesVector(10, 64, 64, 10, 1152, 512),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 10),
        }
      },
      {
        MobActionState::MOVING_DOWN, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 1152, 1472),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
      {
        MobActionState::MOVING_UP, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 1152, 1536),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
      {
        MobActionState::MOVING_LEFT, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 1152, 1600),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
      {
        MobActionState::MOVING_RIGHT, {
          .frames = scene_object::MakeFramesVector(8, 64, 64, 8, 1152, 1664),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 8),
        }
      },
#pragma endregion
    };

    MobState MakeMob(TileMapObjectProps props) {
      constexpr int DEFAULT_HP = 10;

      const Transform t = {
        .position = {props.x, props.y, 0.0},
        .size = {32, 32},
        .scale = {1.0f, 1.0f},
        .rotation_radian = 0,
        .rotation_pivot = {0, 0, 0},
        .position_anchor = {0, 0, 0},
      };

      const UV uv = {
        {1152, 896},
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

      return {
        .transform = t,
        .uv = uv,
        .collider = c,
        .type = MobType::SLIME,
        .state = MobActionState::IDLE_DOWN,
        .is_battle = false,
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

    void UpdateMob(MobState& mob_state, float delta_time) {
      // handle animation end
      if (!mob_state.is_playing) {
        if (is_hurt_state(mob_state.state)) {
          mob_state.state = MobActionState::IDLE_DOWN;
          mob_state.is_loop = true;
          mob_state.is_playing = true;
        }
        if (is_death_state(mob_state.state)) {
          mob_state.is_alive = false;
        }
      }

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

    void HandleDeath(MobState& state) {
      state.state = MobActionState::DEATH_DOWN; // todo: handle facing direction
      state.current_frame = 0;
      state.is_loop = false;
      state.is_playing = true;
      state.current_frame_time = animation_data[MobActionState::DEATH_DOWN].frame_durations[0];
    }

    void HandleHurt(MobState& state) {
      state.state = MobActionState::HURT_DOWN; // todo: handle facing direction
      state.current_frame = 0;
      state.is_loop = false;
      state.is_playing = true;
      state.current_frame_time = animation_data[MobActionState::HURT_DOWN].frame_durations[0];
    }
  }
}
