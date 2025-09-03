module;
#include <cassert>

#include "../input/key_logger.h"
#include "../map/tinyxml/tinyxml2.h"

module game.map.map_manager;
import game.map.tile_repository;
import game.map.field_object;
import game.map.tilemap_object_handler;
import game.map.linked_map;
import game.collision.collider;
import game.scene_manager;

import game.utils.encode;
import game.utils.helper;

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
        // handle ground tile animation only
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

          // do random test
          if (obj.metadata.probability < 1.0f) {
            float prob_test = helper::GetRandomNumberByRange(0.0f, 1.0f);
            if (prob_test > obj.metadata.probability) {
              continue;
            }
          }
        }

        if (obj.metadata.tile_class == "Chest") {
          obj.type = FieldObjectType::CHEST;
        }
      }
      else {
        continue;
      }

      // handle wall and object animation
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

namespace {
  struct MapPath {
    std::string path;
    std::string type;
    int id;
  };

  MapPath ParseMapPathString(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

    std::regex re(R"(([a-zA-Z]+)(\d*)\.tmx)");
    std::smatch match;

    MapPath info;
    info.path = path;
    info.id = 1; // default

    if (std::regex_match(filename, match, re)) {
      info.type = match[1];
      if (match[2].matched && !match[2].str().empty()) {
        info.id = std::stoi(match[2].str());
      }
    }
    return info;
  }
}

std::vector<std::shared_ptr<LinkedMapNode>> MapManager::ExpandMap(std::shared_ptr<LinkedMapNode> map) {
  if (map->data.expired()) return {};
  std::shared_ptr<TileMap> current_map = map->data.lock();
  Vector2 map_position = {current_map->GetTransform().position.x, current_map->GetTransform().position.y};

  std::vector<std::string> keys = helper::GetKeysFromMap(map_data_preloaded_);

  std::vector<std::shared_ptr<LinkedMapNode>> new_added_nodes;
  new_added_nodes.reserve(9);
  map_nodes.reserve(map_nodes.size() + 8);
  tile_maps.reserve(tile_maps.size() + 8);

  if (map->up.expired()) {
    std::string key = helper::GetRandomElement(keys);
    MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
    std::shared_ptr<LinkedMapNode> up_map = std::make_shared<LinkedMapNode>();
    up_map->x = map->x;
    up_map->y = map->y - 1;
    map_nodes.push_back(up_map);
    new_added_nodes.push_back(up_map);

    size_t map_idx = tile_maps.size();
    Vector2 position = {map_position.x, map_position.y - map_height_px_};

    tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
    up_map->data = tile_maps[map_idx];
    map->up = up_map;
    up_map->down = map;
  }
  if (map->down.expired()) {
    std::string key = helper::GetRandomElement(keys);
    MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
    std::shared_ptr<LinkedMapNode> down_map = std::make_shared<LinkedMapNode>();
    down_map->x = map->x;
    down_map->y = map->y + 1;
    map_nodes.push_back(down_map);
    new_added_nodes.push_back(down_map);

    size_t map_idx = tile_maps.size();
    Vector2 position = {map_position.x, map_position.y + map_height_px_};

    tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
    down_map->data = tile_maps[map_idx];
    map->down = down_map;
    down_map->up = map;
  }
  if (map->left.expired()) {
    std::string key = helper::GetRandomElement(keys);
    MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
    std::shared_ptr<LinkedMapNode> left_map = std::make_shared<LinkedMapNode>();
    left_map->x = map->x - 1;
    left_map->y = map->y;
    map_nodes.push_back(left_map);
    new_added_nodes.push_back(left_map);

    size_t map_idx = tile_maps.size();
    Vector2 position = {map_position.x - map_width_px_, map_position.y};

    tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
    left_map->data = tile_maps[map_idx];
    map->left = left_map;
    left_map->right = map;
  }
  if (map->right.expired()) {
    std::string key = helper::GetRandomElement(keys);
    MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
    std::shared_ptr<LinkedMapNode> right_map = std::make_shared<LinkedMapNode>();
    right_map->x = map->x + 1;
    right_map->y = map->y;
    map_nodes.push_back(right_map);
    new_added_nodes.push_back(right_map);

    size_t map_idx = tile_maps.size();
    Vector2 position = {map_position.x + map_width_px_, map_position.y};

    tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
    right_map->data = tile_maps[map_idx];
    map->right = right_map;
    right_map->left = map;
  }
  auto up = map->up.lock();
  auto left = map->left.lock();
  auto right = map->right.lock();
  auto down = map->down.lock();
  if (up && left && right && down) {
    if (up->left.expired()) {
      std::string key = helper::GetRandomElement(keys);
      MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
      std::shared_ptr<LinkedMapNode> new_map = std::make_shared<LinkedMapNode>();
      new_map->x = map->x - 1;
      new_map->y = map->y - 1;
      map_nodes.push_back(new_map);
      new_added_nodes.push_back(new_map);

      size_t map_idx = tile_maps.size();
      Vector2 position = {map_position.x - map_width_px_, map_position.y - map_height_px_};

      tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
      new_map->data = tile_maps[map_idx];
      up->left = new_map;
      left->up = new_map;
      new_map->right = up;
      new_map->down = left;
    }
    if (up->right.expired()) {
      std::string key = helper::GetRandomElement(keys);
      MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
      std::shared_ptr<LinkedMapNode> new_map = std::make_shared<LinkedMapNode>();
      new_map->x = map->x + 1;
      new_map->y = map->y - 1;
      map_nodes.push_back(new_map);
      new_added_nodes.push_back(new_map);

      size_t map_idx = tile_maps.size();
      Vector2 position = {map_position.x + map_width_px_, map_position.y - map_height_px_};

      tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
      new_map->data = tile_maps[map_idx];
      up->right = new_map;
      right->up = new_map;
      new_map->left = up;
      new_map->down = right;
    }
    if (down->left.expired()) {
      std::string key = helper::GetRandomElement(keys);
      MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
      std::shared_ptr<LinkedMapNode> new_map = std::make_shared<LinkedMapNode>();
      new_map->x = map->x - 1;
      new_map->y = map->y + 1;
      map_nodes.push_back(new_map);
      new_added_nodes.push_back(new_map);

      size_t map_idx = tile_maps.size();
      Vector2 position = {map_position.x - map_width_px_, map_position.y + map_height_px_};

      tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
      new_map->data = tile_maps[map_idx];
      down->left = new_map;
      left->down = new_map;
      new_map->right = down;
      new_map->up = left;
    }
    if (down->right.expired()) {
      std::string key = helper::GetRandomElement(keys);
      MapData* map_data = helper::GetRandomElement(map_data_preloaded_[key]).get();
      std::shared_ptr<LinkedMapNode> new_map = std::make_shared<LinkedMapNode>();
      new_map->x = map->x + 1;
      new_map->y = map->y + 1;
      map_nodes.push_back(new_map);
      new_added_nodes.push_back(new_map);

      size_t map_idx = tile_maps.size();
      Vector2 position = {map_position.x + map_width_px_, map_position.y + map_height_px_};

      tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, position));
      new_map->data = tile_maps[map_idx];
      down->right = new_map;
      right->down = new_map;
      new_map->left = down;
      new_map->up = right;
    }
  }
  return new_added_nodes;
}

MapManager::MapManager(GameContext* ctx) {
  TileRepository* tr = SceneManager::GetInstance().GetTileRepository();

  std::string texture_path = SceneManager::GetInstance().GetGameConfig()->map_texture_filepath;
  std::wstring w_texture_path = std::wstring(texture_path.begin(), texture_path.end());
  texture_id_ = ctx->render_resource_manager->texture_manager->Load(w_texture_path);

  // Handle map data, store as cache for generated map 
  auto files = SceneManager::GetInstance().GetGameConfig()->file_paths;
  for (std::string file : files) {
    MapPath map_path = ParseMapPathString(file);

    map_data_preloaded_[map_path.type].push_back(std::make_unique<MapData>(Load(file, tr)));
  }

  std::string default_map = SceneManager::GetInstance().GetGameConfig()->default_map;
  MapData* map_data = map_data_preloaded_[default_map][0].get();

  map_width_px_ = static_cast<float>(map_data->map_width) * map_data->tile_width;
  map_height_px_ = static_cast<float>(map_data->map_height) * map_data->tile_height;

  Vector2 base_position = {
    -map_width_px_ / 2,
    -map_height_px_ / 2,
  };

  size_t map_idx = tile_maps.size();
  tile_maps.emplace_back(std::make_shared<TileMap>(map_data, texture_id_, base_position));

  map_data_preloaded_.erase(default_map);

  std::shared_ptr<LinkedMapNode> base_map_node = std::make_shared<LinkedMapNode>();
  base_map_node->x = 0;
  base_map_node->y = 0;
  base_map_node->data = tile_maps[map_idx];
  map_nodes.push_back(base_map_node);
  ExpandMap(base_map_node);

  active_map_node_ = base_map_node;
}

void MapManager::OnUpdate(GameContext* ctx, float delta_time) {
  for (auto map : GetActiveMaps()) {
    map->OnUpdate(ctx, delta_time);
  }
}

void MapManager::OnRender(GameContext* ctx, Camera* camera) {
  for (auto map : GetActiveMaps()) {
    map->OnRender(ctx, camera);
  }
}

std::shared_ptr<TileMap> MapManager::GetActiveMap() {
  if (auto node = active_map_node_.lock()) {
    if (auto map = node->data.lock())
      return map;
  }
  assert(false);
  return nullptr;
}

std::vector<std::shared_ptr<TileMap>> MapManager::GetActiveMaps() {
  std::vector<std::shared_ptr<TileMap>> maps;
  maps.reserve(9);

  for (auto node : GetActiveLinkedMaps()) {
    auto map = node->data.lock();
    maps.push_back(map);
  }

  return maps;
}

std::shared_ptr<LinkedMapNode> MapManager::GetActiveLinkedMap() {
  auto base_node = active_map_node_.lock();
  return base_node;
}

std::vector<std::shared_ptr<LinkedMapNode>> MapManager::GetActiveLinkedMaps() {
  std::vector<std::shared_ptr<LinkedMapNode>> nodes;
  auto base_node = active_map_node_.lock();
  nodes.push_back(base_node);
  auto up_node = base_node->up.lock();
  nodes.push_back(up_node);
  auto down_node = base_node->down.lock();
  nodes.push_back(down_node);
  auto left_node = base_node->left.lock();
  nodes.push_back(left_node);
  auto right_node = base_node->right.lock();
  nodes.push_back(right_node);
  auto up_left_node = up_node->left.lock();
  nodes.push_back(up_left_node);
  auto up_right_node = up_node->right.lock();
  nodes.push_back(up_right_node);
  auto down_left_node = down_node->left.lock();
  nodes.push_back(down_left_node);
  auto down_right_node = down_node->right.lock();
  nodes.push_back(down_right_node);

  return nodes;
}

void MapManager::ForEachActiveLinkedMapsNode(std::function<void(std::shared_ptr<LinkedMapNode>)> fn) {
  auto nodes = GetActiveLinkedMaps();
  for (auto node : nodes) {
    fn(node);
  }
}

void MapManager::EnterNewMap(std::shared_ptr<LinkedMapNode> node,
                             std::function<void(std::shared_ptr<LinkedMapNode>)> cb) {
  active_map_node_ = node;
  auto new_node = ExpandMap(node);

  for (auto n : new_node) {
    cb(n);
  }
}
