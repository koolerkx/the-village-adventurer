module;

module game.scene_game;

import std;
import game.scene_manager;
import graphic.utils.types;
import game.map.tile_repository;
import game.scene_object;
import game.types;
import game.collision_handler;
import game.map.field_object;
import game.collision.collider;
import game.scene_object.skill;
import game.ui.game_ui;
import game.map.tilemap_object_handler;
import game.player.input.keyboard;

void GameScene::OnEnter(GameContext* ctx) {
  std::cout << "GameScene> OnEnter" << std::endl;

  // Scene
  scene_context.reset(new SceneContext());

  map_ = std::make_unique<TileMap>(ctx);
  scene_context->map = map_.get();

  // Skill
  skill_manager_ = std::make_unique<SkillManager>(ctx);
  scene_context->skill_manager = skill_manager_.get();

  // Player
  std::unique_ptr<KeyboardPlayerInput> player_keyboard_input_ = std::make_unique<KeyboardPlayerInput>(ctx);
  player_ = std::make_unique<Player>(ctx, scene_context.get(), std::move(player_keyboard_input_));
  camera_ = std::make_unique<Camera>();

  // UI
  ui_ = std::make_unique<GameUI>(ctx, scene_context.get(), L"assets/ui.png"); // extract path
  ResetTimer();

  // Mob
  mob_manager_ = std::make_unique<MobManager>();
  for (auto& mob_props : map_->GetMobProps()) {
    mob_manager_->Spawn(mob_props);
  }
}

void GameScene::OnUpdate(GameContext* ctx, float delta_time) {
  // std::cout << "GameScene> OnUpdate: " << delta_time << std::endl;

  map_->OnUpdate(ctx, delta_time);
  player_->OnUpdate(ctx, scene_context.get(), delta_time);
  skill_manager_->OnUpdate(ctx, delta_time);

  UpdateUI(ctx, delta_time);
}

void GameScene::OnFixedUpdate(GameContext* ctx, float delta_time) {
  player_->OnFixedUpdate(ctx, scene_context.get(), delta_time);
  HandlePlayerMovementAndCollisions(delta_time);
  camera_->UpdatePosition(player_->GetPositionVector(), delta_time);

  skill_manager_->OnFixedUpdate(ctx, delta_time);

  ui_->OnFixedUpdate(ctx, scene_context.get(), delta_time);
}

void GameScene::OnRender(GameContext* ctx) {
  // std::cout << "GameScene> OnRender" << std::endl;

  map_->OnRender(ctx, camera_.get());
  player_->OnRender(ctx, scene_context.get(), camera_.get());
  skill_manager_->OnRender(ctx, camera_.get(), player_->GetTransform());

  ui_->OnRender(ctx, scene_context.get(), camera_.get());
}

void GameScene::OnExit(GameContext*) {
  std::cout << "GameScene> OnExit" << std::endl;
}

namespace {
  enum class Axis { X, Y };

  void MoveAndCollideAxis(Player& p, float delta, float v,
                          std::span<Collider<FieldObject>> colliders, Axis axis,
                          auto on_enter_field_object) {
    if (v == 0.0f) return;

    p.SetTransform([&](Transform& t) {
      if (axis == Axis::X) t.position.x += v * delta;
      else t.position.y += v * delta;
    });

    collision::HandleDetection(p.GetCollider(), colliders,
                               [&](Player* player, FieldObject* fo, collision::CollisionResult res) {
                                 player->SetTransform([&](Transform& t) {
                                   if (axis == Axis::X) t.position.x += res.mtv.x;
                                   else t.position.y += res.mtv.y;
                                 });
                                 on_enter_field_object(fo);
                               });
  }
}

void GameScene::HandlePlayerMovementAndCollisions(float delta_time) {
  const auto [x, y] = player_->GetVelocity();
  auto colliders = map_->GetFiledObjectColliders();

  MoveAndCollideAxis(*player_, delta_time, x, colliders, Axis::X,
                     [&](FieldObject* fo) { OnPlayerEnterFieldObject(fo); });
  MoveAndCollideAxis(*player_, delta_time, y, colliders, Axis::Y,
                     [&](FieldObject* fo) { OnPlayerEnterFieldObject(fo); });
}

void GameScene::ResetTimer() {
  timer_elapsed_ = 0;
}

void GameScene::UpdateUI(GameContext* ctx, float delta_time) {
  ui_->SetHpPercentage(player_->GetHPPercentage());

  timer_elapsed_ += delta_time;
  ui_->SetTimerText(timer_elapsed_);

  ui_->OnUpdate(ctx, scene_context.get(), delta_time);
}
