module;

module game.mobs_manager;

import game.mobs.slime;
import graphic.utils.types;
import game.collision_handler;
import game.map.field_object;
import game.map.linked_map;
import game.scene_manager;

void MobManager::Spawn(TileMapObjectProps props) {
  if (props.type == TileMapObjectType::MOB_SLIME) {
    const auto insert_result = mobs_pool_.Insert(mob::slime::MakeMob(props));
    const auto inserted = mobs_pool_.Get(insert_result.value());
    inserted->collider.owner = inserted;              // HACK: workaround handle the object lifecycle
    inserted->attack_range_collider.owner = inserted; // HACK: workaround handle the object lifecycle
    inserted->id = insert_result.value();
  }
}

/**
 * @brief Create trigger area and bind mob inside the trigger
 * @pre MobManager::Spawn
 */
void MobManager::CreateActiveArea(TileMapObjectProps props) {
  std::vector<ObjectPoolIndexType> ids;
  mobs_pool_.ForEach([props, &ids](MobState& mob_state, ObjectPoolIndexType id) {
    if (mob_state.transform.position.x > props.x && mob_state.transform.position.y > props.y && mob_state.transform.
      position.x < props.x + props.width && mob_state.transform.position.y < props.y + props.height) {
      // inside
      ids.push_back(id);
    }
  });

  auto collider = Collider<ActiveArea>{
    .position = {props.x, props.y},
    .shape = RectCollider{
      .x = 0,
      .y = 0,
      .width = props.width,
      .height = props.height
    }
  };

  auto insert_result = active_area_pool_.Insert({collider, ids});
  const auto inserted = active_area_pool_.Get(insert_result.value());
  inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
  inserted->id = static_cast<FixedPoolIndexType>(insert_result.value());

  active_area_state[insert_result.value()] = ActiveAreaState::NOT_COLLIDE;
}

void MobManager::OnUpdate(GameContext*, float delta_time, OnUpdateProps props) {
  // Update Mob State Hitbox
  mobs_pool_.ForEach([delta_time, props](MobState& it) {
    it.attack_cooldown = it.attack_cooldown > 0 ? it.attack_cooldown - delta_time : -1;

    switch (it.type) {
    case MobType::SLIME:
      mob::slime::UpdateMob(it, delta_time, props.player_position);
    default:
      break;
    }

    if (it.is_show_hurt_frame_timer >= 0) {
      it.is_show_hurt_frame_timer -= delta_time;
    }
  });

  // Remove inactive, dead mobs
  mobs_pool_.RemoveIf([&active_area_pool = active_area_pool_](MobState& it) {
    if (!it.is_alive) {
      const auto mob_id = it.id;
      active_area_pool.ForEach([mob_id](ActiveArea& area) {
        if (area.mobs.size() <= 0) return;
        std::erase(area.mobs, mob_id);

        if (area.mobs.size() <= 0) {
          SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_base);
        }
      });
      return true;
    }
    return false;
  });

  // Update Hitbox
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
  // update mobs active state
  active_area_pool_.ForEach(
    [player_collider, &active_area_state = this->active_area_state, &mobs_pool_ = this->mobs_pool_](
    ActiveArea& it, ObjectPoolIndexType id) {
      if (active_area_state[id] == ActiveAreaState::COLLIDE_LAST_FRAME) {
        active_area_state[id] = ActiveAreaState::NOT_COLLIDE;
        // OnExit
        for (ObjectPoolIndexType mob_id : it.mobs) {
          const auto mob = mobs_pool_.Get(mob_id);
          mob->is_battle = false;
        }
        SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_base);
      }
      if (active_area_state[id] == ActiveAreaState::COLLIDING)
        active_area_state[id] = ActiveAreaState::COLLIDE_LAST_FRAME;

      collision::HandleDetection(player_collider, std::span(&it.collider, 1),
                                 [&](Player*, ActiveArea*, collision::CollisionResult) -> void {
                                   if (active_area_state[id] == ActiveAreaState::NOT_COLLIDE) {
                                     // OnEnter
                                     if (it.mobs.size() > 0) {
                                       SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_fight_2);
                                     }
                                     for (ObjectPoolIndexType mob_id : it.mobs) {
                                       const auto mob = mobs_pool_.Get(mob_id);
                                       mob->is_battle = true;
                                     }
                                   }
                                   if (active_area_state[id] == ActiveAreaState::COLLIDE_LAST_FRAME) {
                                     // Colliding
                                   }
                                   active_area_state[id] = ActiveAreaState::COLLIDING;
                                 });
    });

  auto map_colliders = scene_ctx->active_map_node->data.lock()->GetFiledObjectColliders();
  mobs_pool_.ForEach([delta_time, map_colliders, player_collider, this](MobState& it) {
    if (mob::is_death_state(it.state)) return;
    static constexpr float active_range_radius = 512.0f;
    if (math::GetDistance(it.collider.position, player_collider.position) > active_range_radius) return;
#pragma region WALL_COLLISION
    POSITION position_before = it.transform.position;

    if (it.velocity.x * it.velocity.x > 0.0001) {
      it.transform.position.x += it.velocity.x * delta_time;
    }
    SyncCollider(it);
    collision::HandleDetection(it.collider,
                               map_colliders,
                               [&](MobState*, FieldObject*, collision::CollisionResult) -> void {
                                 it.transform.position = position_before;
                                 SyncCollider(it);
                               });

    if (it.velocity.y * it.velocity.y > 0.0001) {
      it.transform.position.y += it.velocity.y * delta_time;
    }
    SyncCollider(it);
    collision::HandleDetection(it.collider,
                               map_colliders,
                               [&](MobState*, FieldObject*, collision::CollisionResult) -> void {
                                 it.transform.position = position_before;
                                 SyncCollider(it);
                               });

    if (!mob::is_moving_state(it.state)) {
      it.velocity.x *= 0.90f;
      it.velocity.y *= 0.90f;
    }
#pragma endregion

    // Handle Attack
    // XXX: Note that attack state will overwrite moving state, the state will jump between two state when its in cooldown
    if (!mob::is_attack_state(it.state) && it.attack_cooldown <= 0) {
      collision::HandleDetection(player_collider, std::span(&it.attack_range_collider, 1),
                                 [this](Player*, MobState* m, collision::CollisionResult) -> void {
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
    if (it.is_show_hurt_frame_timer >= 0) {
      item.color = color::red;
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

int MobManager::MakeDamage(MobState& mob_state, int damage,
                           std::move_only_function<void()> post_action) {
  mob_state.hp -= damage;
  if (mob_state.hp <= 0) {
    // Mob die
    switch (mob_state.type) {
    case MobType::SLIME:
      mob::slime::HandleDeath(mob_state);
    default:
      break;
    }
    post_action();
    return mob_state.hp;
  }

  switch (mob_state.type) {
  // Mob get hurt
  case MobType::SLIME:
    mob::slime::HandleHurt(mob_state);
  default:
    break;
  }
  mob_state.is_show_hurt_frame_timer = 0.1f;
  post_action();

  return mob_state.hp;
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

// should be 1 per map 
std::vector<Collider<ActiveArea>> MobManager::GetActiveAreaColliders() {
  std::vector<Collider<ActiveArea>> colliders;
  colliders.reserve(mobs_pool_.Size());

  active_area_pool_.ForEach([&colliders](const ActiveArea& it) -> void {
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
