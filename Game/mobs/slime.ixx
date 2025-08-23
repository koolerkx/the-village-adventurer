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
      {
        MobActionState::IDLE, {
          .frames = scene_object::MakeFramesVector(6, 64, 64, 6, 1152, 896),
          .frame_durations = scene_object::MakeFramesConstantDuration(0.1f, 6),
        }
      }
    };

    MobState MakeMob(TileMapObjectProps props) {
      return {
        .transform = {
          .position = {props.x, props.y, 0.0},
          .size = {32, 32},
          .scale = {1.0f, 1.0f},
          .rotation_radian = 0,
          .rotation_pivot = {0, 0, 0},
          .position_anchor = {0, 0, 0},
        },
        .uv = {
          {1152, 896},
          {64, 64}
        },
        .collider = {
          .is_trigger = true,
          .position = {props.x, props.y},
          .rotation = 0,
          .rotation_pivot = {0, 0},
          .shape = RectCollider{
            .x = 0, .y = 0,
            .width = 16,
            .height = 16
          }
        },
        .type = MobType::SLIME,
        .state = MobActionState::IDLE,
        .is_battle = false,
      };
    };

    RenderInstanceItem GetRenderInstanceItme(MobState state) {
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
      scene_object::AnimationState anim_state{
        .is_loop = true,
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
