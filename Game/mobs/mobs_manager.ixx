module;

export module game.mobs_manager;

import std;
import game.object_pool;
import graphic.utils.types;
import game.collision.collider;
import game.context;
import game.scene_object.camera;
import game.map.tilemap_object_handler;
import game.types;

export enum class MobActionState: char {
  IDLE_LEFT,
  IDLE_RIGHT,
  IDLE_UP, // default
  IDLE_DOWN,

  ATTACK_LEFT,
  ATTACK_RIGHT,
  ATTACK_UP,
  ATTACK_DOWN,

  HURT_LEFT,
  HURT_RIGHT,
  HURT_UP,
  HURT_DOWN,

  DEATH_LEFT,
  DEATH_RIGHT,
  DEATH_UP,
  DEATH_DOWN,

  MOVING_LEFT,
  MOVING_RIGHT,
  MOVING_UP,
  MOVING_DOWN,

  INACTIVE,
};

export enum class MobType: char {
  NONE,
  SLIME,
};

export struct MobState {
  ObjectPoolIndexType id;
  Transform transform;
  UV uv;
  Collider<MobState> collider;
  MobType type = MobType::NONE;
  MobActionState state = MobActionState::IDLE_DOWN;
  bool is_battle = false; // is in battle

  bool is_alive = true;
  bool is_loop = false;
  bool is_playing = true;
  size_t current_frame = 0;
  float current_frame_time = 0.f;

  Vector2 velocity;
  // mob data
  int hp;
};

export namespace mob {
  bool is_hurt_state(MobActionState state) {
    return state == MobActionState::HURT_DOWN ||
      state == MobActionState::HURT_UP ||
      state == MobActionState::HURT_LEFT ||
      state == MobActionState::HURT_RIGHT;
  }

  bool is_death_state(MobActionState state) {
    return state == MobActionState::DEATH_DOWN ||
      state == MobActionState::DEATH_UP ||
      state == MobActionState::DEATH_LEFT ||
      state == MobActionState::DEATH_RIGHT;
  }

  bool is_moving_state(MobActionState state) {
    return state == MobActionState::MOVING_DOWN ||
      state == MobActionState::MOVING_UP ||
      state == MobActionState::MOVING_LEFT ||
      state == MobActionState::MOVING_RIGHT;
  }
}

export class MobManager {
private:
  FixedPoolIndexType texture_id_;
  ObjectPool<MobState> mobs_pool_;

public:
  MobManager(GameContext* ctx) {
    texture_id_ = ctx->render_resource_manager->texture_manager->Load(L"assets/mobs.png"); // extract path
  }

  void Spawn(TileMapObjectProps);

  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx, Camera* camera);

  void MakeDamage(MobState& mob_state, int damage, const std::move_only_function<void()> post_action);
  void PushBack(MobState& mob_state, Vector2 direction);

  std::vector<Collider<MobState>> GetColliders();
};
