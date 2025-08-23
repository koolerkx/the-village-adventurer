module;

module game.mobs_manager;

import game.mobs.slime;
import graphic.utils.types;

void MobManager::Spawn(TileMapObjectProps props) {
  if (props.type == TileMapObjectType::MOB_SLIME) {
    const auto insert_result = mobs_pool_.Insert(mob::slime::MakeMob(props));
    const auto inserted = mobs_pool_.Get(insert_result.value());
    inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
  }
}

void MobManager::OnUpdate(GameContext* ctx, float delta_time) {
  mobs_pool_.ForEach([delta_time](MobState& it) {
    RenderInstanceItem item;
    switch (it.type) {
    case MobType::SLIME:
      mob::slime::UpdateMob(it, delta_time);
    default:
      break;
    }
  });
}

void MobManager::OnFixedUpdate(GameContext*, float) {}

void MobManager::OnRender(GameContext* ctx, Camera* camera) {
  auto rr = ctx->render_resource_manager->renderer.get();
  std::vector<RenderInstanceItem> render_items;

  render_items.reserve(mobs_pool_.Size());

  mobs_pool_.ForEach([&render_items](MobState& it) {
    RenderInstanceItem item;
    switch (it.type) {
    case MobType::SLIME:
      item = mob::slime::GetRenderInstanceItme(it);
    default:
      break;
    }

    render_items.emplace_back(item);
  });

  rr->DrawSpritesInstanced(render_items, texture_id_, camera->GetCameraProps(), true);
}
