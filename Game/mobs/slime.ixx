module;

export module game.mobs.slime;
import game.mobs_manager;
import game.map.tilemap_object_handler;

export namespace mob {
  namespace slime {
    MobData MakeMobData(TileMapObjectProps props) {
      return {
        .transform_ = {
          .position = {props.x, props.y, 0.0},
          .size = {16, 16},
          .scale = {1.0f, 1.0f},
          .rotation_radian = 0,
          .rotation_pivot = {0, 0, 0},
          .position_anchor = {0, 0, 0},
        },
        .collider_ = {},
        .type = MobType::SLIME,
        .state = MobState::IDLE,
        .is_battle = false,
      };
    };
  }
}
