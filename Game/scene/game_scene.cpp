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
import player.factory;
import game.math;
import game.map.map_manager;
import game.map.linked_map;
import game.result_scene;

void GameScene::OnEnter(GameContext* ctx) {
  ctx->allow_control = false;
  std::cout << "GameScene> OnEnter" << std::endl;

  // Scene
  scene_context.reset(new SceneContext());

  // map_ = std::make_unique<TileMap>(ctx, default_map_position);
  map_manager_ = std::make_unique<MapManager>(ctx);

  // Skill
  skill_manager_ = std::make_unique<SkillManager>(ctx);
  scene_context->skill_manager = skill_manager_.get();
  scene_context->active_map_node = map_manager_->GetActiveLinkedMap();

  // Player
  auto pf = std::make_unique<PlayerFactory>();
  player_ = std::move(pf->Create(ctx));
  camera_ = std::make_unique<Camera>();

  // UI
  ui_ = std::make_unique<GameUI>(ctx, scene_context.get(), L"assets/ui.png"); // extract path
  ResetTimer();

  ui_->SetFadeOverlayAlphaTarget(0.0f, color::black, [&ui = ui_, ctx]() {
    ui->SetUIOpacity(1.0f);
    ctx->allow_control = true;
  });

  // Mob
  mob_manager_ = std::make_unique<MobManager>(ctx);
  for (const auto& mob_props : map_manager_->GetMobProps()) {
    mob_manager_->Spawn(mob_props);
  }
  for (auto& active_area_prop : map_manager_->GetActiveAreaProps()) {
    mob_manager_->CreateActiveArea(active_area_prop);
  }
}

void GameScene::OnUpdate(GameContext* ctx, float delta_time) {
  // std::cout << "GameScene> OnUpdate: " << delta_time << std::endl;
  map_manager_->OnUpdate(ctx, delta_time);
  player_->OnUpdate(ctx, scene_context.get(), delta_time);
  skill_manager_->OnUpdate(ctx, delta_time);
  mob_manager_->OnUpdate(ctx, delta_time, {
                           .player_position = player_->GetPositionVector()
                         });

  UpdateUI(ctx, delta_time);

  if (is_end_) {
    is_end_ = false;
    ctx->allow_control = false;
    std::cerr << "SetFadeOverlayAlphaTarget this=" << static_cast<const void*>(this) << "\n";

    ui_->SetFadeOverlayAlphaTarget(1.0f, color::black, [&timer_elapsed = timer_elapsed_]() {
      SceneManager::GetInstance().ChangeSceneDelayed(std::make_unique<ResultScene>(ResultSceneProps{0, static_cast<float>(timer_elapsed)}));
    });
  }
}

void GameScene::OnFixedUpdate(GameContext* ctx, float delta_time) {
  player_->OnFixedUpdate(ctx, scene_context.get(), delta_time);
  HandlePlayerMovementAndCollisions(delta_time);
  camera_->UpdatePosition(player_->GetPositionVector(), delta_time);

  HandlePlayerEnterMapCollision(delta_time, scene_context.get());

  HandleSkillHitMobCollision(delta_time);
  HandleMobHitPlayerCollision(delta_time);

  ui_->OnFixedUpdate(ctx, scene_context.get(), delta_time);
  skill_manager_->OnFixedUpdate(ctx, delta_time);
  mob_manager_->OnFixedUpdate(ctx, scene_context.get(), delta_time, player_->GetCollider());

  SceneManager::GetInstance().GetAudioManager()->UpdateListenerPosition({
    player_->GetTransform().position.x, player_->GetTransform().position.y
  });
}

void GameScene::OnRender(GameContext* ctx) {
  // std::cout << "GameScene> OnRender" << std::endl;
  map_manager_->OnRender(ctx, camera_.get());
  mob_manager_->OnRender(ctx, camera_.get());
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
  auto colliders = map_manager_->GetFiledObjectColliders();

  MoveAndCollideAxis(*player_, delta_time, x, colliders, Axis::X,
                     [&](FieldObject* fo) { OnPlayerEnterFieldObject(fo); });
  MoveAndCollideAxis(*player_, delta_time, y, colliders, Axis::Y,
                     [&](FieldObject* fo) { OnPlayerEnterFieldObject(fo); });
}

void GameScene::HandlePlayerEnterMapCollision(float, SceneContext* scene_ctx) {
  map_manager_->ForEachActiveLinkedMapsNode(
    [&player = player_, &map_manager = this->map_manager_, &ui = this->ui_, &mob_manager = this->mob_manager_, &
      scene_ctx
    ](std::shared_ptr<LinkedMapNode> node) -> void {
      auto map = node->data.lock();
      if (!map) return;

      auto state = map->GetCollideState();
      auto collider = map->GetMapCollider();

      if (state == CollideState::COLLIDE_LAST_FRAME) {
        // OnExit
        map->SetCollideState(CollideState::NOT_COLLIDE);
      }
      if (state == CollideState::COLLIDING) {
        map->SetCollideState(CollideState::COLLIDE_LAST_FRAME);
      }

      collision::HandleDetection(player->GetCollider(), std::span(&collider, 1),
                                 [&state, &map_manager, &map, &ui, &node, &mob_manager, &scene_ctx]
                               (Player*, TileMap*, collision::CollisionResult) -> void {
                                   if (state == CollideState::NOT_COLLIDE) {
                                     // OnEnter
                                     ui->PlayEnterAreaMessage(map->GetMapName());

                                     scene_ctx->active_map_node = node;
                                     map_manager->EnterNewMap(
                                       node, [&mob_manager, &scene_ctx](std::shared_ptr<LinkedMapNode> new_node) {
                                         auto map = new_node->data.lock();
                                         if (!map) return;
                                         for (const auto& mob_props : map->GetMobProps()) {
                                           mob_manager->Spawn(mob_props);
                                         }
                                         for (auto& active_area_prop : map->GetActiveAreaProps()) {
                                           mob_manager->CreateActiveArea(active_area_prop);
                                         }
                                       });
                                   }
                                   map->SetCollideState(CollideState::COLLIDING);
                                 });
    });
}

void GameScene::HandleSkillHitMobCollision(float) {
  auto mob_colliders = mob_manager_->GetColliders();
  auto skill_colliders = skill_manager_->GetColliders();

  std::span mob_colliders_span{mob_colliders.data(), mob_colliders.size()};
  std::span skill_colliders_span{skill_colliders.data(), skill_colliders.size()};

  auto cb = [&mob_manager = this->mob_manager_, &ui = this->ui_, &player = this->player_]
  (MobState* mob_state, SkillHitbox* skill_hitbox, collision::CollisionResult) -> void {
    if (!skill_hitbox->hit_mobs.contains(mob_state->id) && !mob::is_death_state(mob_state->state)) {
      skill_hitbox->hit_mobs.insert(mob_state->id);
      mob_manager->MakeDamage(*mob_state, skill_hitbox->data->damage, [&]() {
        Vector2 mob_center = {
          mob_state->transform.position.x + mob_state->transform.size.x,
          mob_state->transform.position.y + mob_state->transform.size.y
        };
        Vector2 skill_center = {
          skill_hitbox->transform.position.x + skill_hitbox->transform.size.x / 2,
          skill_hitbox->transform.position.y + skill_hitbox->transform.size.y / 2
        };

        // make damage text
        ui->AddDamageText(
          {
            mob_state->transform.position.x + mob_state->transform.size.x / 2,
            mob_state->transform.position.y + mob_state->transform.size.y / 2
          },
          skill_hitbox->data->name,
          skill_hitbox->data->damage
        );

        // attack push back
        Vector2 dir = math::GetDirection(skill_center, mob_center);

        mob_manager->PushBack(*mob_state, {dir.x, dir.y});
        Vector2 audio_pos = {
          mob_center.x + (dir.x * 12.0f),
          mob_center.y + (dir.y * 12.0f)
        };
        SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::hit_1, audio_pos);
      });
    }
  };

  collision::HandleDetection(mob_colliders_span, skill_colliders_span, cb);
}

void GameScene::HandleMobHitPlayerCollision(float) {
  Collider<Player> player_collider = player_->GetCollider();
  std::vector<Collider<MobHitBox>> mob_hitbox_collider = mob_manager_->GetHitBoxColliders();
  std::span mob_hitbox_collider_span{mob_hitbox_collider.data(), mob_hitbox_collider.size()};

  collision::HandleDetection(player_collider, mob_hitbox_collider_span,
                             [&is_end = is_end_](Player* p, MobHitBox* m, collision::CollisionResult) -> void {
                               if (m->attack_delay >= 0) return;
                               if (m->hit_player) return;
                               m->hit_player = true;
                               m->timeout = 0;

                               float hp = p->Damage(m->damage);
                               SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::hit_2);

                               if (hp <= 0) {
                                 is_end = true;
                               }
                             });
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
