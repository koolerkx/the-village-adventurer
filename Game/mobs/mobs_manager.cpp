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
      item = mob::slime::GetRenderInstanceItem(it);
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
    if (auto collider = std::get_if<RectCollider>(&it.collider.shape)) {
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
  });

  rr->DrawBoxes(rect_view, camera->GetCameraProps(), true);
#endif
}

std::vector<Collider<MobState>> MobManager::GetColliders() {
  std::vector<Collider<MobState>> colliders;
  colliders.reserve(mobs_pool_.Size());

  mobs_pool_.ForEach([&colliders](const MobState& it) -> void {
    colliders.push_back(it.collider);
  });

  return colliders;
}
