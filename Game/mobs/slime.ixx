module;

export module game.mobs.slime;
import game.mobs_manager;
import game.map.tilemap_object_handler;
import game.collision.collider;
import graphic.utils.types;

export namespace mob {
  namespace slime {
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
          // {state.uv.position.x, state.uv.position.y},
          // {state.uv.size.x, state.uv.size.y},
          {1152, 896},
          {64, 64}
        },
        .color = color::white,
      };
    }
  }
}
