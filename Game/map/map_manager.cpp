module;
#include <cassert>
#include "../map/tinyxml/tinyxml2.h"

module game.map.map_manager;
import game.map.tile_repository;
import game.map.field_object;
import game.map.tilemap_object_handler;
import game.collision.collider;
import game.scene_manager;

import game.utils.encode;

import std;

namespace {
  // constexpr std::size_t MAX_WALL_COUNT = 2048; // TODO: extract

  inline void ReadUnsignedAttribute(
    const tinyxml2::XMLElement* element,
    const char* attr_name,
    unsigned* out_value) {
    if (tinyxml2::XMLError result = element->QueryUnsignedAttribute(attr_name, out_value);
      result != tinyxml2::XML_SUCCESS) {
      std::cerr << "Failed to read '" << attr_name << "' attribute: "
        << (element->Attribute(attr_name) ? element->Attribute(attr_name) : "missing")
        << std::endl;
      assert(false);
    }
  }

  inline const char* ReadWStringAttribute(
    const tinyxml2::XMLElement* element,
    const char* attr_name) {
    if (const char* value = element->Attribute(attr_name)) {
      return value;
    }
    std::cerr << "Failed to read '" << attr_name << "' attribute: missing" << std::endl;
    assert(false);
    return nullptr;
  }

  std::vector<TileMapObjectProps> ParseObjectGroup(tinyxml2::XMLElement* mapElement) {
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
}

MapData MapManager::Load(std::string_view filepath, TileRepository* tr) {
  MapData data;

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

  ReadUnsignedAttribute(mapElement, "tilewidth", &data.tile_width);
  ReadUnsignedAttribute(mapElement, "tileheight", &data.tile_height);
  ReadUnsignedAttribute(mapElement, "width", &data.map_width);
  ReadUnsignedAttribute(mapElement, "height", &data.map_height);

  const char* class_name = ReadWStringAttribute(mapElement, "class");

  data.map_name = encode::Utf8ToUtf16(class_name);

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
      std::erase_if(item, ::isspace);
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
    if (tile_ids.size() != data.map_width * data.map_height) {
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

      unsigned int x = (i % data.map_width) * data.tile_width;
      unsigned int y = (i / data.map_height) * data.tile_height;

      TileUV uv = tr->GetUvById(tile_ids[i]);

      if (layer_class != "Object" && layer_class != "Wall") {
        layer.tiles.x.push_back(x);
        layer.tiles.y.push_back(y);
        layer.tiles.u.push_back(uv.u);
        layer.tiles.v.push_back(uv.v);
        layer.tiles.tile_id.push_back(tile_ids[i]);

        // animation data and make tile anim state
        if (std::optional<TileAnimationData> tile_animation_data_result = tr->GetTileAnimatedData(tile_ids[i]);
          tile_animation_data_result.has_value()) {
          TileAnimationData tile_animation_data = tile_animation_data_result.value();

          layer.tile_animation_states_[i] = {
            .is_loop = tile_animation_data.is_loop,
            .play_on_start = tile_animation_data.play_on_start,
            .is_playing = tile_animation_data.play_on_start,
            .frames = tile_animation_data.frames,
            .frame_durations = tile_animation_data.frame_durations
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

        auto result = data.field_object_pool.Insert(std::move(obj));

        if (!result.has_value()) {
          assert(false);
        }
        auto inserted = data.field_object_pool.Get(result.value());
        inserted->collider.owner = inserted; // HACK: workaround handle the object lifecycle
      }
    }
    data.layers.push_back(layer);
  }

  data.map_objects_props = ParseObjectGroup(mapElement);

  return data;
}

MapManager::MapManager(GameContext* ctx) {
  std::string default_map = SceneManager::GetInstance().GetGameConfig()->default_map;
  TileRepository* tr = SceneManager::GetInstance().GetTileRepository();

  std::string default_map_path = "map/map_data/" + default_map + ".tmx";
  std::string texture_path = SceneManager::GetInstance().GetGameConfig()->map_texture_filepath;
  std::wstring w_texture_path = std::wstring(texture_path.begin(), texture_path.end());

  texture_id_ = ctx->render_resource_manager->texture_manager->Load(w_texture_path);

  // todo: store as cache for generated map
  std::unique_ptr<MapData> map_data = std::make_unique<MapData>(Load(default_map_path, tr));

  Vector2 base_position = {
    -(static_cast<float>(map_data->map_width) * map_data->tile_width) / 2,
    -(static_cast<float>(map_data->map_height) * map_data->tile_height) / 2,
  };
  active_map_ = std::make_shared<TileMap>(map_data.get(), texture_id_, base_position);
}

void MapManager::OnUpdate(GameContext* ctx, float delta_time) {
  active_map_->OnUpdate(ctx, delta_time);
}

void MapManager::OnRender(GameContext* ctx, Camera* camera) {
  active_map_->OnRender(ctx, camera);
}
