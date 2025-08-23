module;

export module game.mobs_manager;

import std;
import game.object_pool;
import graphic.utils.types;
import game.collision.collider;
import game.context;
import game.scene_object.camera;
import game.map.tilemap_object_handler;

export enum class MobActionState: char {
  IDLE,
  ATTACK,
  HURT,
  DEATH,
  MOVING,
  INACTIVE,
};

export enum class MobType: char {
  NONE,
  SLIME,
};

export struct MobState {
  Transform transform;
  UV uv;
  Collider<MobState> collider;
  MobType type = MobType::NONE;
  MobActionState state = MobActionState::IDLE;
  bool is_battle = false; // is in battle
  
  bool is_playing = true;
  size_t current_frame = 0;
  float current_frame_time = 0.f;
};

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
};
