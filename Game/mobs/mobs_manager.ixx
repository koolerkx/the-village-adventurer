module;

export module game.mobs_manager;

import std;
import game.object_pool;
import graphic.utils.types;
import game.collision.collider;
import game.context;
import game.scene_object.camera;
import game.map.tilemap_object_handler;

export enum class MobState: char {
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

export struct MobData {
  Transform transform_;
  Collider<MobData> collider_;
  MobType type = MobType::NONE;
  MobState state = MobState::IDLE;
  bool is_battle = false; // is in battle
};

export class MobManager {
private:
  ObjectPool<MobData> object_pool_;

public:
  MobManager() {}

  void Spawn(TileMapObjectProps);

  // void OnUpdate(GameContext* ctx, float delta_time);
  // void OnFixedUpdate(GameContext* ctx, float delta_time);
  // void OnRender(GameContext* ctx, Camera* camera, Transform player_transform);
};
