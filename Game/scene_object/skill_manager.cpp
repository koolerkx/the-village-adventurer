module;
#include <cassert>

module game.scene_object.skill;

const std::wstring texture_path = L"assets/attack.png"; // TODO: extract

SkillManager::SkillManager(GameContext* ctx) {
  scene_object::LoadTexture(texture_id, texture_path, ctx->render_resource_manager->texture_manager.get());
}

void SkillManager::OnUpdate(GameContext*, float delta_time) {
  hitbox_pool_.RemoveIf([](SkillHitbox& hitbox) {
    return !hitbox.is_playing;
  });

  hitbox_pool_.ForEach([delta_time](SkillHitbox& hitbox, ObjectPoolIndexType) {
    scene_object::AnimationState state{
      .is_loop = false,
      .play_on_start = false,
      .is_playing = hitbox.is_playing,
      .frames = hitbox.data->frames,
      .frame_durations = hitbox.data->frame_durations,
      .current_frame = hitbox.current_frame,
      .current_frame_time = hitbox.current_frame_time
    };

    scene_object::UpdateAnimation(state, delta_time, hitbox.uv);

    hitbox.current_frame = state.current_frame;
    hitbox.current_frame_time = state.current_frame_time;
    hitbox.is_playing = state.is_playing;
  });
}

void SkillManager::OnFixedUpdate(GameContext*, float) {}

void SkillManager::OnRender(GameContext* ctx, Camera* camera, Transform player_transform) {
  auto rr = ctx->render_resource_manager->renderer.get();

  std::vector<RenderInstanceItem> render_items;
  render_items.reserve(hitbox_pool_.Size());

  hitbox_pool_.ForEach([&render_items, player_transform](SkillHitbox& it) {
    if (it.data->is_stick_to_player) {
      it.transform.position.x = player_transform.position.x;
      it.transform.position.y = player_transform.position.y;

      it.collider.position.x = player_transform.position.x + it.data->base_transform.position_anchor.x;
      it.collider.position.y = player_transform.position.y + it.data->base_transform.position_anchor.y;
    }

    render_items.emplace_back(RenderInstanceItem{
      .transform = it.transform,
      .uv = {
        {it.uv.position.x, it.uv.position.y},
        {it.uv.size.x, it.uv.size.y},
      },
      .color = color::white,
    });
  });

  rr->DrawSpritesInstanced(render_items, texture_id, camera->GetCameraProps(), true);

#if defined(DEBUG) || defined(_DEBUG)
  // DEBUG render collider
  std::vector<Rect> rect_view;
  rect_view.reserve(hitbox_pool_.Size());

  hitbox_pool_.ForEach([&](const SkillHitbox& hitbox) {
    auto& collider = hitbox.collider;

    if (std::holds_alternative<RectCollider>(collider.shape)) {
      const auto& shape = std::get<RectCollider>(collider.shape);

      std::array<Vector2, 4> rotated = scene_object::GetRotatedPoints({shape.x, shape.y, shape.width, shape.height},
                                                                 {
                                                                   shape.base_width / 2 + collider.rotation_pivot.x,
                                                                   shape.base_height / 2 + collider.rotation_pivot.y
                                                                 },
                                                                 collider.rotation);

      rect_view.push_back({
        {
          rotated[0].x + collider.position.x,
          rotated[0].y + collider.position.y, 0
        },
        {
          rotated[2].x + collider.position.x,
          rotated[2].y + collider.position.y, 0
        },
        color::red
      });

      const std::array<POSITION, 4> points = {
        POSITION{rotated[0].x + collider.position.x, rotated[0].y + collider.position.y, 0},
        POSITION{rotated[1].x + collider.position.x, rotated[1].y + collider.position.y, 0},
        POSITION{rotated[2].x + collider.position.x, rotated[2].y + collider.position.y, 0},
        POSITION{rotated[3].x + collider.position.x, rotated[3].y + collider.position.y, 0}
      };
      ctx->render_resource_manager->renderer->DrawPolygon(points, color::red, camera->GetCameraProps(), true);
    }
  });
#endif
}

void SkillManager::PlaySkill(SKILL_TYPE type, Vector2 position, float rotation) {
  auto result = skill_data.find(type);
  if (result == skill_data.end()) { return; }
  auto& data = result->second;

  auto transform = data.base_transform;
  transform.position = {position.x, position.y, 0};
  transform.rotation_radian += rotation;

  TileUV tile_uv = data.frames[0];

  auto uv = UV{
    {static_cast<float>(tile_uv.u), static_cast<float>(tile_uv.v)},
    {static_cast<float>(tile_uv.w), static_cast<float>(tile_uv.h)}
  };

  auto collider = Collider<SkillHitbox>{
    .is_trigger = true,
    .position = {
      transform.position.x + transform.position_anchor.x, transform.position.y + transform.position_anchor.y
    },
    .rotation = transform.rotation_radian,
    .rotation_pivot = {data.base_transform.rotation_pivot.x, data.base_transform.rotation_pivot.y},
    .owner = nullptr,
    .shape = RectCollider{
      data.base_collider_padding.left,
      data.base_collider_padding.top,
      transform.size.x - data.base_collider_padding.left - data.base_collider_padding.right,
      transform.size.y - data.base_collider_padding.top - data.base_collider_padding.bottom,
      transform.size.x,
      transform.size.y
    }
  };

  SkillHitbox hitbox = {
    .transform = transform,
    .uv = uv,
    .collider = collider,
    .current_frame = 0,
    .current_frame_time = 0,
    .data = &data
  };

  auto insert_result = hitbox_pool_.Insert(std::move(hitbox));
  if (!insert_result.has_value()) {
    assert(false);
  }
  auto inserted = hitbox_pool_.Get(insert_result.value());
  inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
}

std::vector<Collider<SkillHitbox>> SkillManager::GetColliders() {
  std::vector<Collider<SkillHitbox>> colliders;
  colliders.reserve(hitbox_pool_.Size());

  hitbox_pool_.ForEach([&colliders](const SkillHitbox& it) -> void {
    colliders.push_back(it.collider);
  });

  return colliders;
}
