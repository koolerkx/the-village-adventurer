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

void GameScene::OnEnter(GameContext* ctx) {
  std::cout << "GameScene> OnEnter" << std::endl;

  std::string default_map = SceneManager::GetInstance().GetGameConfig()->default_map;
  TileRepository* tr = SceneManager::GetInstance().GetTileRepository();

  map_.reset(new TileMap());
  std::string default_map_path = "map/map_data/" + default_map + ".tmx";

  std::string texture_path = SceneManager::GetInstance().GetGameConfig()->map_texture_filepath;
  std::wstring w_texture_path = std::wstring(texture_path.begin(), texture_path.end());

  FixedPoolIndexType id = ctx->render_resource_manager->texture_manager->Load(w_texture_path);
  map_->Load(default_map_path, id, tr);
  Transform t = map_->GetTransform();

  // TODO: remove debug data
  t.position.x = -128.0f;
  t.position.y = -256.0f;
  t.scale.x = 1.0f;
  t.scale.y = 1.0f;
  t.position_anchor.x = 0.0f;
  t.position_anchor.y = 0.0f;
  map_->SetTransform(t);

  player_ = std::make_unique<Player>(ctx, scene_context.get());
  camera_ = std::make_unique<Camera>();

  // Scene
  scene_context.reset(new SceneContext());
  scene_context->map = map_.get();

  // Skill
  skill_manager_ = std::make_unique<SkillManager>(ctx);
  scene_context->skill_manager = skill_manager_.get();

  // UI
  ui_ = std::make_unique<GameUI>(ctx, scene_context.get(), L"assets/ui.png"); // extract path

  time_at_start_ = std::chrono::high_resolution_clock::now();
}

void GameScene::OnUpdate(GameContext* ctx, float delta_time) {
  // std::cout << "GameScene> OnUpdate: " << delta_time << std::endl;
  ui_->SetHpPercentage(player_->GetHPPercentage());

  map_->OnUpdate(ctx, delta_time);
  player_->OnUpdate(ctx, scene_context.get(), delta_time);
  skill_manager_->OnUpdate(ctx, delta_time);
  
  ui_->OnUpdate(ctx, scene_context.get(), delta_time);

  auto now = std::chrono::high_resolution_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - time_at_start_);

  auto minutes = static_cast<int>(elapsed.count() / 60);
  auto seconds = static_cast<int>(elapsed.count() % 60);

  ui_->SetTimerText(minutes, seconds);
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

void GameScene::HandlePlayerMovementAndCollisions(float delta_time) {
  const Vector2 velocity = player_->GetVelocity();

  // movement
  std::span<Collider<FieldObject>> field_objects = map_->GetFiledObjectColliders();

  player_->SetTransform([=](Transform& t) {
    t.position.x += velocity.x * delta_time;
  });
  collision::HandleDetection(player_->GetCollider(), field_objects,
                             [&](Player* player, FieldObject* fo, collision::CollisionResult result) {
                               player->SetTransform([result](Transform& t) {
                                 t.position.x += result.mtv.x;
                               });

                               OnPlayerEnterFieldObject(fo);
                             });

  player_->SetTransform([=](Transform& t) {
    t.position.y += velocity.y * delta_time;
  });
  collision::HandleDetection(player_->GetCollider(), field_objects,
                             [&](Player* player, FieldObject* fo, collision::CollisionResult result) {
                               player->SetTransform([result](Transform& t) {
                                 t.position.y += result.mtv.y;
                               });

                               OnPlayerEnterFieldObject(fo);
                             });
}

void GameScene::ResetTimer() {
  time_at_start_ = std::chrono::high_resolution_clock::now();
}
