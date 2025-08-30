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
import game.scene_game.context;
import game.player;
import game.math;
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
  Collider<MobState> attack_range_collider;
  MobType type = MobType::NONE;
  MobActionState state = MobActionState::IDLE_DOWN;
  Vector2 inactive_position; // back to this position when inactive
  bool is_battle = false;    // is in battle

  bool is_alive = true;
  bool is_loop = false;
  bool is_playing = true;
  size_t current_frame = 0;
  float current_frame_time = 0.f;

  float attack_cooldown;
  float moving_cooldown;

  float is_show_hurt_frame_timer = 0.0f;
  
  Vector2 velocity;
  // mob data
  int hp;
};

export struct ActiveArea {
  Collider<ActiveArea> collider;
  std::vector<ObjectPoolIndexType> mobs;
  FixedPoolIndexType id;
};

enum class ActiveAreaState {
  NOT_COLLIDE,
  COLLIDING,
  COLLIDE_LAST_FRAME
};

export namespace mob {
  bool is_idle_state(MobActionState state) {
    return state == MobActionState::IDLE_DOWN ||
      state == MobActionState::IDLE_UP ||
      state == MobActionState::IDLE_LEFT ||
      state == MobActionState::IDLE_RIGHT;
  }

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

  bool is_attack_state(MobActionState state) {
    return state == MobActionState::ATTACK_DOWN ||
      state == MobActionState::ATTACK_UP ||
      state == MobActionState::ATTACK_LEFT ||
      state == MobActionState::ATTACK_RIGHT;
  }
  bool is_position_reset(Vector2 mob_pos, Vector2 inactive_pos) {
    constexpr float DISTANCE_THRESHOLD = 32.0f;
    if (math::GetDistance(mob_pos, inactive_pos) < DISTANCE_THRESHOLD) {
      return true;
    }
    return false;
  }
}

export struct MobHitBox {
  Transform transform;
  UV uv;
  Collider<MobHitBox> collider;
  float damage = 10;
  // animation
  size_t current_frame = 0;
  float current_frame_time = 0.f;
  float attack_delay = attack_delay;
  float timeout = 9999.0f;
  bool is_playing = false;
  bool is_animated = false;

  bool hit_player; // hit player only once
};

export struct OnUpdateProps {
  Vector2 player_position{};
};

export class MobManager {
private:
  FixedPoolIndexType texture_id_;
  ObjectPool<MobState> mobs_pool_;
  ObjectPool<MobHitBox> mob_hitbox_pool_;
  ObjectPool<ActiveArea> active_area_pool_;
  std::unordered_map<ObjectPoolIndexType, ActiveAreaState> active_area_state;

  void SyncCollider(MobState& mob_state);

public:
  MobManager(GameContext* ctx) {
    texture_id_ = ctx->render_resource_manager->texture_manager->Load(L"assets/mobs.png"); // extract path
  }

  void Spawn(TileMapObjectProps);
  void CreateActiveArea(TileMapObjectProps);

  void OnUpdate(GameContext* ctx, float delta_time, OnUpdateProps props);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time, Collider<Player> player_collider);
  void OnRender(GameContext* ctx, Camera* camera);

  int MakeDamage(MobState& mob_state, int damage, const std::move_only_function<void()> post_action);
  void PushBack(MobState& mob_state, Vector2 direction);

  std::vector<Collider<MobState>> GetColliders();
  std::vector<Collider<MobHitBox>> GetHitBoxColliders();
  std::vector<Collider<ActiveArea>> GetActiveAreaColliders();
};
