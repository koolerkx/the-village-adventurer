module;

module game.mobs_manager;

import game.mobs.slime;
import graphic.utils.types;
import game.collision_handler;
import game.map.field_object;

void MobManager::Spawn(TileMapObjectProps props) {
  if (props.type == TileMapObjectType::MOB_SLIME) {
    const auto insert_result = mobs_pool_.Insert(mob::slime::MakeMob(props));
    const auto inserted = mobs_pool_.Get(insert_result.value());
    inserted->collider.owner = inserted;              // HACK: workaround handle the object lifecycle
    inserted->attack_range_collider.owner = inserted; // HACK: workaround handle the object lifecycle
    inserted->id = insert_result.value();
  }
}

void MobManager::OnUpdate(GameContext* ctx, float delta_time) {
  mobs_pool_.ForEach([delta_time](MobState& it) {
    it.attack_cooldown = it.attack_cooldown > 0 ? it.attack_cooldown - delta_time : -1;

    switch (it.type) {
    case MobType::SLIME:
      mob::slime::UpdateMob(it, delta_time);
    default:
      break;
    }
  });

  mobs_pool_.RemoveIf([](MobState& it) {
    return !it.is_alive;
  });

  mob_hitbox_pool_.ForEach([delta_time](MobHitBox& it) {
    it.timeout -= delta_time;
    it.attack_delay -= delta_time;
  });

  mob_hitbox_pool_.RemoveIf([](MobHitBox& it) {
    return !it.is_playing && it.timeout <= 0;
  });
}

void MobManager::OnFixedUpdate(GameContext*, SceneContext* scene_ctx, float delta_time,
                               Collider<Player> player_collider) {
  auto map_colliders = scene_ctx->map->GetFiledObjectColliders();

  // handle moving
  mobs_pool_.ForEach([delta_time, map_colliders, player_collider, this](MobState& it) {
    if (mob::is_death_state(it.state)) return;

    POSITION position_before = it.transform.position;

    if (it.velocity.x * it.velocity.x > 0.0001) {
      it.transform.position.x += it.velocity.x * delta_time;
    }
    SyncCollider(it);
    collision::HandleDetection(it.collider,
                               map_colliders,
                               [&](MobState* m, FieldObject* fo, collision::CollisionResult res) -> void {
                                 it.transform.position = position_before;
                                 SyncCollider(it);
                               });

    if (it.velocity.y * it.velocity.y > 0.0001) {
      it.transform.position.y += it.velocity.y * delta_time;
    }
    SyncCollider(it);
    collision::HandleDetection(it.collider,
                               map_colliders,
                               [&](MobState* m, FieldObject* fo, collision::CollisionResult res) -> void {
                                 it.transform.position = position_before;
                                 SyncCollider(it);
                               });

    if (!mob::is_moving_state(it.state)) {
      it.velocity.x *= 0.90f;
      it.velocity.y *= 0.90f;
    }

    // Handle Attack
    if (!mob::is_attack_state(it.state) && it.attack_cooldown <= 0) {
      collision::HandleDetection(player_collider, std::span(&it.attack_range_collider, 1),
                                 [this](Player* p, MobState* m, collision::CollisionResult res) -> void {
                                   m->attack_cooldown = 2.0f;

                                   m->state = MobActionState::ATTACK_DOWN;
                                   m->current_frame = 0;
                                   m->current_frame_time = 0;
                                   m->is_playing = true;
                                   m->is_loop = false;

                                   switch (m->type) {
                                   case MobType::SLIME: {
                                     auto insert_result = mob_hitbox_pool_.Insert(mob::slime::GetHitBox(*m));
                                     const auto inserted = mob_hitbox_pool_.Get(insert_result.value());
                                     inserted->collider.owner = inserted;
                                     break;
                                   }
                                   default:
                                     break;
                                   }
                                 });
    }
  });
}

void MobManager::OnRender(GameContext* ctx, Camera* camera) {
  auto rr = ctx->render_resource_manager->renderer.get();
  std::vector<RenderInstanceItem> render_items;

  render_items.reserve(mobs_pool_.Size());

  mobs_pool_.ForEach([&render_items](MobState& it) {
    RenderInstanceItem item;
    switch (it.type) {
    case MobType::SLIME:
      item = mob::slime::GetRenderInstanceItem(it);
      break;
    default:
      break;
    }

    render_items.emplace_back(item);
  });

  rr->DrawSpritesInstanced(render_items, texture_id_, camera->GetCameraProps(), true);

#if defined(DEBUG) || defined(_DEBUG)
  std::vector<Rect> rect_view;
  rect_view.reserve(mobs_pool_.Size());

  mobs_pool_.ForEach([&rect_view, rr, camera](MobState& it) {
    if (std::get_if<RectCollider>(&it.collider.shape)) {
      const auto& shape = std::get<RectCollider>(it.collider.shape);

      rect_view.push_back(Rect{
        {
          it.collider.position.x + shape.x,
          it.collider.position.y + shape.y, 0
        },
        {
          it.collider.position.x + shape.x + shape.width,
          it.collider.position.y + shape.y + shape.height, 0
        },
        color::red
      });
    }
    else {
      const auto& shape = std::get<CircleCollider>(it.collider.shape);
      rr->DrawLineCircle({
                           it.collider.position.x + shape.x,
                           it.collider.position.y + shape.y, 0
                         },
                         shape.radius,
                         color::red, camera->GetCameraProps());
    }

    const auto& shape = std::get<CircleCollider>(it.attack_range_collider.shape);
    rr->DrawLineCircle({
                         it.collider.position.x + shape.x,
                         it.collider.position.y + shape.y, 0
                       },
                       shape.radius,
                       color::blue, camera->GetCameraProps());
  });

  mob_hitbox_pool_.ForEach([&rect_view, rr, camera](MobHitBox it) -> void {
    if (it.attack_delay >= 0) return;
    if (std::get_if<RectCollider>(&it.collider.shape)) {
      const auto& shape = std::get<RectCollider>(it.collider.shape);

      rect_view.push_back(Rect{
        {
          it.collider.position.x + shape.x,
          it.collider.position.y + shape.y, 0
        },
        {
          it.collider.position.x + shape.x + shape.width,
          it.collider.position.y + shape.y + shape.height, 0
        },
        color::yellow
      });
    }
    else {
      const auto& shape = std::get<CircleCollider>(it.collider.shape);
      rr->DrawLineCircle({
                           it.collider.position.x + shape.x,
                           it.collider.position.y + shape.y, 0
                         },
                         shape.radius,
                         color::yellow, camera->GetCameraProps());
    }
  });

  rr->DrawBoxes(rect_view, camera->GetCameraProps(), true);
#endif
}

void MobManager::MakeDamage(MobState& mob_state, int damage,
                            std::move_only_function<void()> post_action) {
  mob_state.hp -= damage;
  if (mob_state.hp <= 0) {
    switch (mob_state.type) {
    case MobType::SLIME:
      mob::slime::HandleDeath(mob_state);
    default:
      break;
    }
    post_action();
    return;
  }

  switch (mob_state.type) {
  case MobType::SLIME:
    mob::slime::HandleHurt(mob_state);
  default:
    break;
  }
  post_action();
}

void MobManager::PushBack(MobState& mob_state, Vector2 direction) {
  constexpr float speed = 75;

  // give impulse velocity
  mob_state.velocity.x = direction.x * speed;
  mob_state.velocity.y = direction.y * speed;
}

std::vector<Collider<MobState>> MobManager::GetColliders() {
  std::vector<Collider<MobState>> colliders;
  colliders.reserve(mobs_pool_.Size());

  mobs_pool_.ForEach([&colliders](const MobState& it) -> void {
    colliders.push_back(it.collider);
  });

  return colliders;
}

std::vector<Collider<MobHitBox>> MobManager::GetHitBoxColliders() {
  std::vector<Collider<MobHitBox>> colliders;
  colliders.reserve(mobs_pool_.Size());

  mob_hitbox_pool_.ForEach([&colliders](const MobHitBox& it) -> void {
    colliders.push_back(it.collider);
  });

  return colliders;
}

void MobManager::SyncCollider(MobState& mob_state) {
  switch (mob_state.type) {
  case MobType::SLIME:
    mob::slime::SyncCollider(mob_state);
    break;
  default:
    break;
  }
}
