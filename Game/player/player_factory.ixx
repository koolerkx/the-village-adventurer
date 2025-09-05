export module player.factory;

import std;
import game.context;
import game.player;
import game.player.input.keyboard;
import game.scene_object;
import game.types;
import graphic.utils.types;
import game.collision.collider;
import game.player.input.xinput;
import game.player.input.composite;

std::unordered_map<PlayerState, scene_object::AnimationFrameData> animation_data{
  {
    PlayerState::IDLE_LEFT, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 32),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::IDLE_RIGHT, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 32 * 2),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::IDLE_UP, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 0),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::IDLE_DOWN, {
      .frames = scene_object::MakeFramesVector(1, 32, 32, 1, 32, 32 * 3),
      .frame_durations = scene_object::MakeFramesConstantDuration(9999.0f, 1),
    }
  },
  {
    PlayerState::MOVE_LEFT, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3, 0, 32),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  },
  {
    PlayerState::MOVE_RIGHT, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3, 0, 32 * 2),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  },
  {
    PlayerState::MOVE_UP, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  },
  {
    PlayerState::MOVE_DOWN, {
      .frames = scene_object::MakeFramesVector(3, 32, 32, 3, 0, 32 * 3),
      .frame_durations = scene_object::MakeFramesConstantDuration(0.15f, 3),
    }
  }
};

export class PlayerFactory {
public:
  std::unique_ptr<Player> Create(GameContext* ctx) {
    std::unique_ptr<XInputPlayerInput> player_xinput_input = std::make_unique<XInputPlayerInput>(ctx);
    std::unique_ptr<KeyboardPlayerInput> player_keyboard_input = std::make_unique<KeyboardPlayerInput>(ctx);
    std::unique_ptr<CompositePlayerInput> composite_player_input = std::make_unique<CompositePlayerInput>(
      std::move(player_xinput_input), std::move(player_keyboard_input));

    const auto tm = ctx->render_resource_manager->texture_manager.get();
    static const std::wstring texture_path = L"assets/character_01.png";
    FixedPoolIndexType texture_id_ = tm->Load(texture_path);

    auto player = std::make_unique<Player>(texture_id_, std::move(composite_player_input), animation_data);

    return std::move(player);
  }
};
