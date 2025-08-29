module;
#include <cassert>

module game.map;
import game.map.tile_repository;
import graphic.utils.types;
import game.scene_object.camera;
import game.map.tilemap_object_handler;
import game.scene_manager;
import game.utils.encode;

/**
 * Constructor copy the map data to have its own data
 * @param map_data 
 * @param texture_id 
 * @param base_position 
 */
TileMap::TileMap(MapData* map_data, FixedPoolIndexType texture_id, Vector2 base_position) {
  texture_id_ = texture_id;

  map_name_ = map_data->map_name;
  tile_width_ = map_data->tile_width;
  tile_height_ = map_data->tile_height;
  map_width_ = map_data->map_width;
  map_height_ = map_data->map_height;
  layers_ = map_data->layers;
  map_objects_props_ = map_data->map_objects_props;

  map_data->field_object_pool.ForEach([&field_object_pool = field_object_pool_](const FieldObject& fo) -> void {
    auto result = field_object_pool.Insert(fo);
    if (!result.has_value()) {
      assert(false);
    }
    auto inserted = field_object_pool.Get(result.value());
    inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
  });

  Transform t = GetTransform();
  t.position.x = base_position.x;
  t.position.y = base_position.y;
  t.scale.x = 1.0f;
  t.scale.y = 1.0f;
  t.position_anchor.x = 0.0f;
  t.position_anchor.y = 0.0f;

  SetTransform(t);

  // Map Collider
  map_collider_ = Collider<TileMap>{
    .is_trigger = true,
    .position = {base_position},
    .owner = this,
    .shape = RectCollider{
      .x = 0, .y = 0,
      .width = static_cast<float>(map_width_ * tile_width_),
      .height = static_cast<float>(map_height_ * tile_height_),
    }
  };
}

void TileMap::OnUpdate(GameContext*, float delta_time) {
  for (auto& layer : layers_) {
    for (auto& [index, state] : layer.tile_animation_states_) {
      if (!state.is_playing || state.frames.empty()) continue;
      state.current_frame_time += delta_time;

      if (state.current_frame_time < state.frame_durations[state.current_frame]) continue;

      // next frame
      state.current_frame_time -= state.frame_durations[state.current_frame];
      state.current_frame++;

      // end frame handle
      if (state.current_frame >= state.frames.size()) {
        if (state.is_loop) {
          state.current_frame = 0;
        }
        else {
          state.current_frame = state.frames.size() - 1;
          state.is_playing = false;
        }
      }

      layer.tiles.u[index] = state.frames[state.current_frame].u;
      layer.tiles.v[index] = state.frames[state.current_frame].v;
    }
  }

  field_object_pool_.ForEach([delta_time](FieldObject& field_object) {
      auto& state = field_object.animation_state;
      if (!state.is_playing || state.frames.empty()) return;
      state.current_frame_time += delta_time;

      if (state.current_frame_time < state.frame_durations[state.current_frame]) return;

      // next frame
      state.current_frame_time -= state.frame_durations[state.current_frame];
      state.current_frame++;

      // end frame handle
      if (state.current_frame >= state.frames.size()) {
        if (state.is_loop) {
          state.current_frame = 0;
        }
        else {
          state.current_frame = state.frames.size() - 1;
          state.is_playing = false;
        }
      }

      field_object.tile.uv.u = state.frames[state.current_frame].u;
      field_object.tile.uv.v = state.frames[state.current_frame].v;
    }
  );
}

void TileMap::OnRender(GameContext* ctx, Camera* camera) {
  std::vector<RenderInstanceItem> render_items;
  for (auto& layer : layers_) {
    size_t size = layer.tiles.tile_id.size();

    render_items.reserve(size);

    for (size_t i = 0; i < size; ++i) {
      if (layer.tiles.tile_id[i] < 0) continue;
      render_items.emplace_back(RenderInstanceItem{
        .transform = {
          .position = {
            layer.tiles.x[i] * transform_.scale.x + transform_.position.x,
            layer.tiles.y[i] * transform_.scale.y + transform_.position.y,
            transform_.position.z
          },
          .size = {
            static_cast<float>(tile_width_),
            static_cast<float>(tile_height_)
          },
          .scale = transform_.scale,
          .position_anchor = transform_.position_anchor,
        },
        .uv = {
          {static_cast<float>(layer.tiles.u[i]), static_cast<float>(layer.tiles.v[i])},
          {static_cast<float>(tile_width_), static_cast<float>(tile_height_)},
        },
        .color = color::white
      });
    }

    ctx->render_resource_manager->renderer->DrawSpritesInstanced(
      std::span<RenderInstanceItem>(render_items.data(), render_items.size()),
      texture_id_,
      camera->GetCameraProps(),
      true);
    render_items.clear();
  }

  // Field Object
  field_object_pool_.ForEach(
    [&render_items, transform = transform_, tile_width = tile_width_, tile_height = tile_height_](
    FieldObject& field_object) {
      render_items.emplace_back(RenderInstanceItem{
        .transform = {
          .position = {
            field_object.position.x,
            field_object.position.y,
            transform.position.z
          },
          .size = {
            static_cast<float>(tile_width),
            static_cast<float>(tile_height)
          },
          .scale = transform.scale,
          .position_anchor = transform.position_anchor,
        },
        .uv = {
          {static_cast<float>(field_object.tile.uv.u), static_cast<float>(field_object.tile.uv.v)},
          {static_cast<float>(field_object.tile.uv.w), static_cast<float>(field_object.tile.uv.h)},
        },
        .color = color::white
      });
    });

  ctx->render_resource_manager->renderer->DrawSpritesInstanced(
    std::span<RenderInstanceItem>(render_items.data(), render_items.size()),
    texture_id_,
    camera->GetCameraProps(),
    true);
  render_items.clear();

#if defined(DEBUG) || defined(_DEBUG)
  // DEBUG render collider
  std::vector<Rect> rect_view;
  rect_view.reserve(field_object_pool_.Size());

  field_object_pool_.ForEach([&](FieldObject& field_object) {
    auto& collider = field_object.collider;
    if (std::holds_alternative<RectCollider>(collider.shape)) {
      const auto& shape = std::get<RectCollider>(collider.shape);
      rect_view.push_back({
        {collider.position.x + shape.x, collider.position.y + shape.y, 0},
        {collider.position.x + shape.x + shape.width, collider.position.y + shape.y + shape.height, 0},
        color::red
      });
    }
  });

  ctx->render_resource_manager->renderer->DrawBoxes(rect_view,
                                                    camera->GetCameraProps(),
                                                    true);
#endif
}
