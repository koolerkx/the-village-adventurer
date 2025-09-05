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
import game.utils.helper;
import game.player.buff;
import game.title_scene;
import game.player.level;

void GameScene::OnEnter(GameContext* ctx) {
  ctx->allow_control = false;
  std::cout << "GameScene> OnEnter" << std::endl;

  SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_base);

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
  ui_->InitSkillData(AVAILABLE_SKILLS);

  ui_->SetFadeOverlayAlphaTarget(0.0f, color::black, [&ui = ui_, ctx, this]() {
    ui->SetUIOpacity(1.0f);
    ctx->allow_control = true;
    this->ResetTimer();
    is_allow_pause_ = true;
  });

  // Pause Menu
  pause_menu_ui_ = std::make_unique<PauseMenuUI>(ctx); // extract path

  // Level up UI
  level_up_ui_ = std::make_unique<LevelUpUI>(ctx);

  // Status UI
  status_ui_ = std::make_unique<StatusUI>(ctx);

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
  UpdateInput(ctx->input_handler);

  if (scene_context->vibration_timeout >= 0) {
    scene_context->vibration_timeout -= delta_time;
  }
  else {
    ctx->input_handler->StopXInputVibration();
  }

  if (input.is_pause_menu_button && ui_throttle.CanCall() && is_allow_pause_) {
    input.is_pause_menu_button = false;
    pause_menu_ui_->Reset();
    is_pause_ = true;
    is_allow_pause_ = false;
    is_allow_status_ui_control_ = false;
    SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::select_se_1);
    SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_pause_menu);
  }

  if (is_pause_) {
    HandlePauseMenu(ctx, delta_time);
    return;
  }

  if (input.is_status_menu_button && ui_throttle.CanCall() && is_allow_pause_ && !is_show_status_ui_) {
    input.is_status_menu_button = false;
    HandleOnStatusOpen(ctx, delta_time);
  }

  if (is_show_status_ui_) {
    HandleStatusUpdate(ctx, delta_time);
    return;
  }

  if (player_->GetLevel() != player_level_prev_) {
    player_level_prev_ = player_->GetLevel();
    level_up_selected_option_ = 1;
    level_up_ui_->Reset();
    is_show_level_up_ui = true;
    is_allow_level_up_ui_control_ = false;
    is_allow_pause_ = false;

    SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::select_se_1);
    SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_pause_menu);

    auto elems = helper::GetRandomElements<3>(player_level::option_list);
    level_up_ui_->SetOptionData(elems);
    level_up_options_ = elems;

    level_up_ui_->SetFadeInWithCallback([&allow = is_allow_level_up_ui_control_]() {
      allow = true;
    });
  }

  if (is_show_level_up_ui) {
    HandleLevelUpUI(ctx, delta_time);
    return;
  }

  map_manager_->OnUpdate(ctx, delta_time);
  player_->OnUpdate(ctx, scene_context.get(), delta_time);
  skill_manager_->OnUpdate(ctx, delta_time);
  mob_manager_->OnUpdate(ctx, delta_time, {
                           .player_position = player_->GetPositionVector(),
                           .is_player_invincible = player_->GetIsInvincible()
                         });

  UpdateUI(ctx, delta_time);

  if (is_end_) {
    is_end_ = false;
    ctx->allow_control = false;

    ui_->SetFadeOverlayAlphaTarget(1.0f, color::black,
                                   [&timer_elapsed = timer_elapsed_, &monster_killed = monster_killed_]() {
                                     SceneManager::GetInstance().ChangeSceneDelayed(
                                       std::make_unique<ResultScene>(ResultSceneProps{
                                         monster_killed, static_cast<float>(timer_elapsed)
                                       }));
                                   });
  }
}

void GameScene::OnFixedUpdate(GameContext* ctx, float delta_time) {
  if (is_pause_) {
    SceneManager::GetInstance().GetAudioManager()->StopWalking();
    pause_menu_ui_->OnFixedUpdate(ctx, delta_time);
    return;
  }
  if (is_show_status_ui_) {
    SceneManager::GetInstance().GetAudioManager()->StopWalking();
    status_ui_->OnFixedUpdate(ctx, delta_time);
    return;
  }
  if (is_show_level_up_ui) {
    SceneManager::GetInstance().GetAudioManager()->StopWalking();
    level_up_ui_->OnFixedUpdate(ctx, delta_time);
    return;
  }

  player_->OnFixedUpdate(ctx, scene_context.get(), delta_time);
  HandlePlayerMovementAndCollisions(ctx, delta_time);
  camera_->UpdatePosition(player_->GetPositionVector(), delta_time);

  HandlePlayerEnterMapCollision(delta_time, scene_context.get());

  HandleSkillHitMobCollision(ctx, delta_time);
  HandleMobHitPlayerCollision(ctx, delta_time);
  HandleSkillHitWallCollision(ctx, delta_time);

  ui_->OnFixedUpdate(ctx, scene_context.get(), delta_time);
  skill_manager_->OnFixedUpdate(ctx, delta_time, player_->GetTransform());
  mob_manager_->OnFixedUpdate(ctx, scene_context.get(), delta_time, player_->GetCollider(), player_->GetIsInvincible());

  SceneManager::GetInstance().GetAudioManager()->UpdateListenerPosition({
    player_->GetTransform().position.x, player_->GetTransform().position.y
  });
}

void GameScene::OnRender(GameContext* ctx) {
  map_manager_->OnRender(ctx, camera_.get());
  mob_manager_->OnRender(ctx, camera_.get());
  player_->OnRender(ctx, scene_context.get(), camera_.get());
  skill_manager_->OnRender(ctx, camera_.get());

  ui_->OnRender(ctx, scene_context.get(), camera_.get());

  // Pause menu overlay
  if (is_pause_) {
    pause_menu_ui_->OnRender(ctx, camera_.get());
  }
  if (is_show_level_up_ui) {
    level_up_ui_->OnRender(ctx, camera_.get());
  }

  if (is_show_status_ui_) {
    status_ui_->OnRender(ctx, camera_.get());
  }
}

void GameScene::OnExit(GameContext* ctx) {
  ctx->input_handler->StopXInputVibration();
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

void GameScene::HandlePlayerMovementAndCollisions(GameContext* ctx, float delta_time) {
  const auto [x, y] = player_->GetVelocity();
  auto colliders = map_manager_->GetFiledObjectColliders();

  std::function<void(FieldObject&)> on_chest_open = [&](FieldObject& fo) {
    chest::RewardType reward_type = chest::GetRandomRewardType();

    switch (reward_type) {
    case chest::RewardType::HEAL:
      player_->Heal(helper::GetRandomNumberByOffset(10.0f, 5.0f));
      break;
    case chest::RewardType::BUFF_ATTACK_POWER: {
      PlayerBuff pb;
      pb.type = BuffType::ATTACK_POWER;
      player_->AddBuff(pb);
      break;
    }
    // case chest::RewardType::BUFF_ATTACK_SPEED: {
    //   PlayerBuff pb;
    //   pb.type = BuffType::ATTACK_SPEED;
    //   player_->AddBuff(pb);
    //   break;
    // }
    case chest::RewardType::BUFF_MOVING_SPEED: {
      PlayerBuff pb;
      pb.type = BuffType::MOVING_SPEED;
      pb.duration = 5.0f;
      pb.multiplier = 1.25f;
      player_->AddBuff(pb);
      break;
    }
    case chest::RewardType::INVINCIBLE: {
      PlayerBuff pb;
      pb.type = BuffType::INVINCIBLE;
      pb.duration = 10.0f;
      player_->AddBuff(pb);
      SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_action_1);
      break;
    }
    default:
      break;
    }

    SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::chest_open, fo.position, 1.0f);
    ui_->AddEventText(chest::GetChestRewardEventText(reward_type), chest::GetChestRewardEventColor(reward_type));
    ui_->AddLogText(chest::GetChestLogText(reward_type), color::lightGreenA400);

    Vector2 star_pos = {
      fo.position.x + 8, fo.position.y + 8
    };

    ui_->AddExperienceCoin(star_pos, BASE_CHEST_OPEN_EXP, [&](int value) {
      player_->AddExperience(value);
    });
  };

  std::function<void(FieldObject&)> on_collide_wall = [&](FieldObject& fo) {
    ctx->input_handler->SetXInputVibration(VIBRATION_LOWER_LOW, VIBRATION_LOW);
    scene_context->vibration_timeout = 0.1f;
  };

  MoveAndCollideAxis(*player_, delta_time, x, colliders, Axis::X,
                     [on_collide_wall, on_chest_open](FieldObject* fo) {
                       OnPlayerEnterFieldObject(fo, on_collide_wall, on_chest_open);
                     });
  MoveAndCollideAxis(*player_, delta_time, y, colliders, Axis::Y,
                     [on_collide_wall, on_chest_open](FieldObject* fo) {
                       OnPlayerEnterFieldObject(fo, on_collide_wall, on_chest_open);
                     });
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

void GameScene::HandleSkillHitMobCollision(GameContext* ctx, float) {
  auto mob_colliders = mob_manager_->GetColliders();
  auto skill_colliders = skill_manager_->GetColliders();

  std::span mob_colliders_span{mob_colliders.data(), mob_colliders.size()};
  std::span skill_colliders_span{skill_colliders.data(), skill_colliders.size()};

  float damage_multiplier = GetBuffMultiplier(player_->GetBuffs(), BuffType::ATTACK_POWER)
    * player_level::GetLevelAbilityMultiplier(player_->GetLevelUpAbilities(), player_level::Ability::ATTACK)
    + player_level::GetLevelAbilityValue(player_->GetLevelUpAbilities(), player_level::Ability::ATTACK);;

  auto cb = [&mob_manager = this->mob_manager_, &ui = this->ui_, &player = this->player_, &monster_killed =
      monster_killed_, &skill_manager = skill_manager_, damage_multiplier, &ctx, &scene_ctx = scene_context]
  (MobState* mob_state, SkillHitbox* skill_hitbox, collision::CollisionResult) -> void {
    if (!skill_hitbox->hit_mobs.contains(mob_state->id) && !mob::is_death_state(mob_state->state)) {
      float damage = skill_hitbox->data->damage * damage_multiplier;
      Vector2 mob_center = {
        mob_state->transform.position.x + mob_state->transform.size.x / 2,
        mob_state->transform.position.y + mob_state->transform.size.y / 2
      };

      skill_hitbox->hit_mobs.insert(mob_state->id);
      int remain_hp = mob_manager->MakeDamage(*mob_state, static_cast<int>(damage), [&]() {
        Vector2 skill_center = {
          skill_hitbox->transform.position.x + skill_hitbox->transform.size.x / 2,
          skill_hitbox->transform.position.y + skill_hitbox->transform.size.y / 2
        };

        // make damage text
        ui->AddDamageText(
          mob_center,
          skill_hitbox->data->name,
          static_cast<short>(damage)
        );
        std::wstringstream wss;
        wss << skill_hitbox->data->name << L" で " << mob::GetMobName(mob_state->type) << L" に " <<
          static_cast<short>(damage) << L" ダメージを与えた";
        ui->AddLogText(wss.str(), color::cyanA400);

        if (skill_hitbox->data->is_destroy_by_mob) {
          skill_manager->HandleDestroyCollision(skill_hitbox, ctx, scene_ctx->vibration_timeout);
        }

        // attack push back
        Vector2 dir = math::GetDirection(skill_center, {
                                           mob_state->transform.position.x + mob_state->transform.size.x,
                                           mob_state->transform.position.y + mob_state->transform.size.y
                                         });

        mob_manager->PushBack(*mob_state, {dir.x, dir.y});
        Vector2 audio_pos = {
          mob_center.x + (dir.x * 12.0f),
          mob_center.y + (dir.y * 12.0f)
        };
        SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::hit_1, audio_pos);
      });
      if (remain_hp <= 0) {
        monster_killed++;

        ui->AddExperienceCoin(mob_center, BASE_MONSTER_KILL_EXP, [&](int value) {
          player->AddExperience(value);
        });
      }
    }
  };

  collision::HandleDetection(mob_colliders_span, skill_colliders_span, cb);
}

void GameScene::HandleMobHitPlayerCollision(GameContext* ctx, float) {
  Collider<Player> player_collider = player_->GetCollider();
  std::vector<Collider<MobHitBox>> mob_hitbox_collider = mob_manager_->GetHitBoxColliders();
  std::span mob_hitbox_collider_span{mob_hitbox_collider.data(), mob_hitbox_collider.size()};

  collision::HandleDetection(player_collider, mob_hitbox_collider_span,
                             [&is_end = is_end_, &ui = ui_,
                               &ctx, &scene_ctx = scene_context
                             ](Player* p, MobHitBox* m, collision::CollisionResult) -> void {
                               if (m->attack_delay >= 0) return;
                               if (m->hit_player) return;

                               m->hit_player = true;
                               m->timeout = 0;

                               if (p->GetIsInvincible()) return;

                               std::wstringstream wss;
                               wss << L"プレイヤーが "
                                 << std::fixed << std::setprecision(2) << m->damage
                                 << L" ダメージを受けた！";

                               ui->AddLogText(wss.str(),
                                              color::redA700);

                               float hp = p->Damage(m->damage);
                               SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(
                                 audio_clip::hit_2, p->GetPositionVector());
                               scene_ctx->vibration_timeout = 0.1f;
                               ctx->input_handler->SetXInputVibration(VIBRATION_HALF, VIBRATION_HALF);

                               if (hp <= 0) {
                                 is_end = true;
                               }
                             });

  // handle invincible
  if (player_->GetIsInvincible()) {
    std::vector<Collider<MobState>> mob_colliders = mob_manager_->GetColliders();
    std::span mob_colliders_span{mob_colliders.data(), mob_colliders.size()};
    collision::HandleDetection(player_collider, mob_colliders_span,
                               [&mob_manager = mob_manager_, &ui = ui_, &monster_killed = monster_killed_, &player =
                                 player_, &ctx, &scene_ctx = scene_context](
                               Player* p, MobState* mob_state, collision::CollisionResult) -> void {
                                 if (mob_state->hp <= 0) return;
                                 if (mob::is_death_state(mob_state->state)) return;

                                 Vector2 mob_center = {
                                   mob_state->transform.position.x + mob_state->transform.size.x / 2,
                                   mob_state->transform.position.y + mob_state->transform.size.y / 2
                                 };

                                 mob_manager->MakeDamage(*mob_state, 999, [&]() {
                                   // make damage text
                                   ui->AddDamageText(
                                     mob_center,
                                     L"無敵",
                                     999
                                   );
                                   ui->AddLogText(L"無敵状態の力で " + mob::GetMobName(mob_state->type) + L" を一撃で倒した！",
                                                  color::cyanA400);

                                   SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(
                                     audio_clip::hit_1, p->GetPositionVector());
                                   scene_ctx->vibration_timeout = 0.1f;
                                   ctx->input_handler->SetXInputVibration(VIBRATION_HIGH, VIBRATION_HIGH);
                                 });
                                 monster_killed++;

                                 ui->AddExperienceCoin(mob_center, BASE_MONSTER_KILL_EXP, [&](int value) {
                                   // FIXME: now using workaround since player in callback seems cannot be access correctly
                                   player->AddExperience(value);
                                 });
                               }
    );
  }
}

void GameScene::HandleSkillHitWallCollision(GameContext* ctx, float) {
  auto map_colliders = map_manager_->GetFiledObjectColliders();

  std::vector<Collider<SkillHitbox>> skill_colliders{};
  for (auto s : skill_manager_->GetColliders()) {
    if (s.owner->data->is_destroy_by_wall)
      skill_colliders.push_back(s);
  }

  std::span<Collider<SkillHitbox>> skill_colliders_span{skill_colliders.data(), skill_colliders.size()};
  std::span<Collider<FieldObject>> map_colliders_span{map_colliders.data(), map_colliders.size()};

  collision::HandleDetection(skill_colliders_span, map_colliders_span,
                             [&skill_manager = skill_manager_, &ctx, &scene_ctx = scene_context](
                             SkillHitbox* skill, FieldObject*,
                             collision::CollisionResult) -> void {
                               skill_manager->HandleDestroyCollision(skill, ctx, scene_ctx->vibration_timeout);
                             });
}

void GameScene::HandlePauseMenu(GameContext* ctx, float delta_time) {
  auto am = SceneManager::GetInstance().GetAudioManager();

  constexpr int options_count = 2;
  if (input.is_button_up && ui_throttle.CanCall()) {
    pause_menu_selected_option_ = (pause_menu_selected_option_ + 1) % options_count;
    am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
  }
  if (input.is_button_down && ui_throttle.CanCall()) {
    pause_menu_selected_option_ = (pause_menu_selected_option_ - 1 + options_count) % options_count;
    am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
  }
  pause_menu_ui_->SetSelectedOption(pause_menu_selected_option_);

  if (input.is_pause_menu_button && ui_throttle.CanCall()) {
    am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
    is_pause_ = false;
    is_allow_pause_ = true;
    is_allow_status_ui_control_ = true;
    SceneManager::GetInstance().GetAudioManager()->PlayPreviousBGM();
    return;
  }

  if (input.is_button_yes && ui_throttle.CanCall()) {
    am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);

    if (pause_menu_selected_option_ == 0) {
      // back to game
      is_pause_ = false;
      is_allow_pause_ = true;
      is_allow_status_ui_control_ = true;
      SceneManager::GetInstance().GetAudioManager()->PlayPreviousBGM();
    }
    else {
      // back to title
      is_pause_ = false;
      is_allow_pause_ = false;
      ctx->allow_control = false;

      ui_->SetFadeOverlayAlphaTarget(1.0f, color::white, []() {
        SceneManager::GetInstance().ChangeSceneDelayed(
          std::make_unique<TitleScene>());
      });
    }
  }

  pause_menu_ui_->OnUpdate(ctx, delta_time);
}

void GameScene::HandleLevelUpUI(GameContext* ctx, float delta_time) {
  auto am = SceneManager::GetInstance().GetAudioManager();

  if (is_allow_level_up_ui_control_) {
    constexpr int options_count = 3;
    if (input.is_button_right && ui_throttle.CanCall()) {
      level_up_selected_option_ = (level_up_selected_option_ + 1) % options_count;
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }
    if (input.is_button_left && ui_throttle.CanCall()) {
      level_up_selected_option_ = (level_up_selected_option_ - 1 + options_count) % options_count;
      am->PlayAudioClip(audio_clip::keyboard_click, {0, 0}, 0.25);
    }
    level_up_ui_->SetSelectedOption(level_up_selected_option_);

    if (input.is_button_yes && ui_throttle.CanCall()) {
      am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
      HandleLevelUpSelection(level_up_options_[level_up_selected_option_]);
      is_show_level_up_ui = false;
      is_allow_level_up_ui_control_ = false;
      is_allow_pause_ = true;
    }
  }

  level_up_ui_->OnUpdate(ctx, delta_time);
}

void GameScene::HandleLevelUpSelection(player_level::OptionType type) {
  switch (type) {
  case player_level::OptionType::ATTACK:
    player_->AddLevelUpAbility({
      player_level::Ability::ATTACK,
      1.2f,
    });
    break;
  case player_level::OptionType::DEFENSE:
    player_->AddLevelUpAbility({
      player_level::Ability::DEFENSE,
      1.2f,
    });
    break;
  case player_level::OptionType::MOVING_SPEED:
    player_->AddLevelUpAbility({
      player_level::Ability::MOVING_SPEED,
      1.05f,
    });
    break;
  case player_level::OptionType::HP_UP:
    player_->AddLevelUpAbility({
      player_level::Ability::HP_UP,
      1.0f,
      10
    });
    break;
  case player_level::OptionType::HEAL:
    player_->Heal(player_->GetMaxHp() * 0.5f);
    break;
  }
}

void GameScene::HandleOnStatusOpen(GameContext*, float) {
  is_show_status_ui_ = true;
  is_allow_pause_ = false;

  SceneManager::GetInstance().GetAudioManager()->PlayAudioClip(audio_clip::select_se_1);
  SceneManager::GetInstance().GetAudioManager()->PlayBGM(audio_clip::bgm_pause_menu);

  status_ui_->Active({
                       .hp = player_->GetHp(),
                       .max_hp = player_->GetMaxHp(),
                       .defense = player_->GetDefense(),
                       .attack = player_->GetAttack(),
                       .speed = player_->GetSpeed(),
                       .experience = player_->GetExperience(),
                       .max_experience = player_->GetMaxExperience(),
                       .total_experience = player_->GetTotalExperience(),
                       .level = player_->GetLevel(),
                       .abilities = player_->GetLevelUpAbilities(),
                       .buffs = player_->GetBuffs(),
                       .monster_killed = monster_killed_,
                       .timer_elapsed = timer_elapsed_
                     }, [&allow = is_allow_status_ui_control_]() {
                       allow = true;
                     });
}

void GameScene::HandleStatusUpdate(GameContext* ctx, float delta_time) {
  auto am = SceneManager::GetInstance().GetAudioManager();
  status_ui_->OnUpdate(ctx, delta_time);

  if (is_allow_status_ui_control_ && (input.is_button_yes || input.is_status_menu_button) && ui_throttle.CanCall()) {
    am->PlayAudioClip(audio_clip::equip_3, {0, 0}, 0.75);
    am->PlayPreviousBGM();
    is_show_status_ui_ = false;
    is_allow_status_ui_control_ = false;
    is_allow_pause_ = true;
  }
}

void GameScene::ResetTimer() {
  timer_elapsed_ = 0;
}

void GameScene::UpdateUI(GameContext* ctx, float delta_time) {
  ui_->SetHpPercentage(player_->GetHPPercentage());

  timer_elapsed_ += delta_time;
  ui_->SetTimerText(timer_elapsed_);

  ui_->SetSkillSelected(player_->GetSelectedSkillId());
  ui_->UpdatePlayerBuffs(player_->GetBuffs());

  ui_->OnUpdate(ctx, scene_context.get(), delta_time, camera_.get());
  ui_->SetExperienceBarPercentage(player_->GetExperiencePercentage());
  ui_->SetPlayerLevel(player_->GetLevel());
}

void GameScene::UpdateInput(InputHandler* ih) {
  auto x_input_analog = ih->GetXInputAnalog();

  input.is_button_yes = ih->IsXInputButtonDown(XButtonCode::A)
    || ih->IsKeyDown(KeyCode::KK_SPACE) || ih->IsKeyDown(KeyCode::KK_ENTER);
  input.is_button_no = ih->IsXInputButtonDown(XButtonCode::B) || ih->IsKeyDown(KeyCode::KK_ESCAPE);

  input.is_button_up = ih->GetXInputButton(XButtonCode::DPadUp) || x_input_analog.first.second > 0
    || ih->GetKey(KeyCode::KK_W) || ih->GetKey(KeyCode::KK_UP);
  input.is_button_down = ih->GetXInputButton(XButtonCode::DPadDown) || x_input_analog.first.second < 0
    || ih->GetKey(KeyCode::KK_S) || ih->GetKey(KeyCode::KK_DOWN);

  input.is_button_left = ih->GetXInputButton(XButtonCode::DPadLeft) || x_input_analog.first.first < 0
    || ih->GetKey(KeyCode::KK_A) || ih->GetKey(KeyCode::KK_LEFT);
  input.is_button_right = ih->GetXInputButton(XButtonCode::DPadRight) || x_input_analog.first.first > 0
    || ih->GetKey(KeyCode::KK_D) || ih->GetKey(KeyCode::KK_RIGHT);

  input.is_button_left_2 = ih->IsKeyDown(KeyCode::KK_Q)
    || ih->IsXInputButtonDown(XButtonCode::LB) || ih->IsXInputButtonDown(XButtonCode::Y);
  input.is_button_right_2 = ih->IsKeyDown(KeyCode::KK_E)
    || ih->IsXInputButtonDown(XButtonCode::RB) || ih->IsXInputButtonDown(XButtonCode::B);

  input.is_pause_menu_button = ih->IsKeyDown(KeyCode::KK_ESCAPE) || ih->IsXInputButtonDown(XButtonCode::Start);
  input.is_status_menu_button = ih->IsKeyDown(KeyCode::KK_I) || ih->IsKeyDown(KeyCode::KK_R)
    || ih->IsXInputButtonDown(XButtonCode::X);
}
