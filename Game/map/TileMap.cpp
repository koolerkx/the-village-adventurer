module;
#include <cassert>
#include "../map/tinyxml/tinyxml2.h"

module game.map;
import game.map.tile_repository;
import graphic.utils.types;
import game.scene_object.camera;
import game.map.tilemap_object_handler;

TileMap::TileMap() {}

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

void TileMap::Load(std::string_view filepath, FixedPoolIndexType texture_id, TileRepository* tr) {
  texture_id_ = texture_id;
  tinyxml2::XMLDocument doc;
  if (doc.LoadFile(filepath.data()) != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to load XML file: " + std::string(filepath) << std::endl;
    assert(false);
  }

  auto* mapElement = doc.FirstChildElement("map");
  if (!mapElement) {
    std::cerr << "Necessary element not found: map" << std::endl;
    assert(false);
  }

  if (tinyxml2::XMLError result = mapElement->QueryUnsignedAttribute("tilewidth", &tile_width_);
    result != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to read 'tilecount' attribute: "
      << (mapElement->Attribute("tilecount") ? mapElement->Attribute("tilecount") : "missing")
      << std::endl;
    assert(false);
  }

  if (tinyxml2::XMLError result = mapElement->QueryUnsignedAttribute("tileheight", &tile_height_);
    result != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to read 'tileheight' attribute: "
      << (mapElement->Attribute("tileheight") ? mapElement->Attribute("tileheight") : "missing")
      << std::endl;
    assert(false);
  }

  if (tinyxml2::XMLError result = mapElement->QueryUnsignedAttribute("width", &map_width_);
    result != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to read 'width' attribute: "
      << (mapElement->Attribute("width") ? mapElement->Attribute("width") : "missing")
      << std::endl;
    assert(false);
  }

  if (tinyxml2::XMLError result = mapElement->QueryUnsignedAttribute("height", &map_height_);
    result != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to read 'height' attribute: "
      << (mapElement->Attribute("height") ? mapElement->Attribute("height") : "missing")
      << std::endl;
    assert(false);
  }

  // Layer
  for (auto* layerElement = mapElement->FirstChildElement("layer"); layerElement; layerElement = layerElement->
       NextSiblingElement("layer")) {
    MapLayer layer;
    std::string layer_class = layerElement->Attribute("class") ? layerElement->Attribute("class") : "";

    auto* dataElement = layerElement->FirstChildElement("data");
    if (!dataElement || !dataElement->GetText()) {
      continue;
    }

    std::string csvData = dataElement->GetText();
    std::vector<int> tile_ids;
    std::stringstream ss(csvData);
    std::string item;

    // Read csv cell by cell
    // remove space if any
    // convert unsigned int
    while (std::getline(ss, item, ',')) {
      item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
      if (!item.empty()) {
        try {
          unsigned int tile_id = std::stoul(item);
          tile_ids.push_back((tile_id - 1));
        }
        catch (const std::exception&) {
          std::cerr << "Invalid tile id: " + item << std::endl;
          assert(false);
        }
      }
    }

    // validation check
    if (tile_ids.size() != map_width_ * map_height_) {
      std::cerr << "Layer tile data not match map size" << std::endl;
      assert(false);
    }

    for (unsigned int i = 0; i < tile_ids.size(); ++i) {
      if (tile_ids[i] < 0) {
        layer.tiles.x.push_back(0);
        layer.tiles.y.push_back(0);
        layer.tiles.u.push_back(0);
        layer.tiles.v.push_back(0);
        layer.tiles.tile_id.push_back(-1);
        continue;
      }

      unsigned int x = (i % map_width_) * tile_width_;
      unsigned int y = (i / map_width_) * tile_height_;

      TileUV uv = tr->GetUvById(tile_ids[i]);

      if (layer_class != "Object" && layer_class != "Wall") {
        layer.tiles.x.push_back(x);
        layer.tiles.y.push_back(y);
        layer.tiles.u.push_back(uv.u);
        layer.tiles.v.push_back(uv.v);
        layer.tiles.tile_id.push_back(tile_ids[i]);

        // animation data and make tile anim state
        if (std::optional<TileAnimationData> tile_animation_data = tr->GetTileAnimatedData(tile_ids[i]);
          tile_animation_data.has_value()) {
          TileAnimationData data = tile_animation_data.value();

          layer.tile_animation_states_[i] = {
            .is_loop = data.is_loop,
            .play_on_start = data.play_on_start,
            .is_playing = data.play_on_start,
            .frames = data.frames,
            .frame_durations = data.frame_durations
          };
        }

        continue;
      }

      // handle field object

      FieldObject obj;

      obj.position = {static_cast<float>(x), static_cast<float>(y)};
      obj.tile = {uv};

      if (layer_class == "Wall") {
        obj.type = FieldObjectType::WALL;
      }
      else if (layer_class == "Object") {
        std::optional<TileMetaData> tile_metadata = tr->GetTileMetadata(tile_ids[i]);
        if (tile_metadata.has_value()) {
          obj.metadata = tile_metadata.value();
        }

        if (obj.metadata.tile_class == "Chest") {
          obj.type = FieldObjectType::CHEST;
        }
      }
      else {
        continue;
      }

      std::optional<TileAnimationData> tile_animation_data = tr->GetTileAnimatedData(tile_ids[i]);
      if (tile_animation_data.has_value()) {
        auto anim_data = tile_animation_data.value();
        obj.animation_state = TileAnimationState{
          .is_loop = anim_data.is_loop,
          .is_playing = anim_data.play_on_start,
          .frames = anim_data.frames,
          .frame_durations = anim_data.frame_durations
        };
      }

      std::optional<std::vector<CollisionData>> tile_collision_data = tr->GetTileCollisionData(tile_ids[i]);
      if (!tile_collision_data.has_value()) continue;
      auto collision_data = tile_collision_data.value();

      for (int k = 0; k < collision_data.size(); k++) {
        ColliderShape shape;
        if (collision_data[k].is_circle) {
          shape = CircleCollider{
            .x = static_cast<float>(collision_data[k].x),
            .y = static_cast<float>(collision_data[k].y),
            .radius = static_cast<float>(collision_data[k].width) // 通常 width 為 radius
          };
        }
        else {
          shape = RectCollider{
            .x = static_cast<float>(collision_data[k].x),
            .y = static_cast<float>(collision_data[k].y),
            .width = static_cast<float>(collision_data[k].width),
            .height = static_cast<float>(collision_data[k].height)
          };
        }

        obj.collider = Collider<FieldObject>{
          .position = {static_cast<float>(x), static_cast<float>(y)},
          .rotation = 0,
          .owner = nullptr,
          .shape = shape,
        };

        auto result = field_object_pool_.Insert(std::move(obj));

        if (!result.has_value()) {
          assert(false);
        }
        auto inserted = field_object_pool_.Get(result.value());
        inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
      }
    }
    layers_.push_back(layer);
  }

  map_objects_props_ = ParseObjectGroup(mapElement);
}

std::vector<TileMapObjectProps> TileMap::ParseObjectGroup(tinyxml2::XMLElement* mapElement) {
  std::vector<TileMapObjectProps> map_object;

  // for each object in each object group
  for (auto* objectGroup = mapElement->FirstChildElement("objectgroup");
       objectGroup; objectGroup = objectGroup->NextSiblingElement("objectgroup")) {
    for (auto* object = objectGroup->FirstChildElement("object");
         object; object = object->NextSiblingElement("object")) {
      TileMapObjectProps props;

      props.x = object->FloatAttribute("x", 0.0f);
      props.y = object->FloatAttribute("y", 0.0f);
      props.width = object->FloatAttribute("width", 0.0f);
      props.height = object->FloatAttribute("height", 0.0f);

      std::string name = object->Attribute("name") ? object->Attribute("name") : "";
      std::string type = object->Attribute("type") ? object->Attribute("type") : "";
      props.type = tilemap_object_handler::MapTileMapObject(type, name);

      map_object.push_back(props);
    }
  }
  return map_object;
}
