module;
#include "tinyxml/tinyxml2.h"
#include <cassert>

module game.map.tile_repository;

TileRepository::TileRepository(std::string_view metadata_filepath) {
  tinyxml2::XMLDocument doc;
  if (doc.LoadFile(metadata_filepath.data()) != tinyxml2::XML_SUCCESS) {
    std::cerr << "Failed to load XML file: " + std::string(metadata_filepath) << std::endl;
    assert(false);
  }

  auto* tileset = doc.FirstChildElement("tileset");
  if (!tileset) {
    std::cerr << "Necessary element not found: tileset" << std::endl;
    assert(false);
  }

  unsigned int temp;
  if (tileset->QueryUnsignedAttribute("tilecount", &tileCount) != tinyxml2::XML_SUCCESS) {
    std::cerr << "Necessary element missing or invalid: tilecount" << std::endl;
    assert(false);
  }

  if (tileset->QueryUnsignedAttribute("tilewidth", &temp) != tinyxml2::XML_SUCCESS || temp > std::numeric_limits<
    unsigned short>::max()) {
    std::cerr << "Necessary element missing or invalid: tilewidth" << std::endl;
    assert(false);
  }
  tile_width_ = static_cast<unsigned short>(temp);

  if (tileset->QueryUnsignedAttribute("tileheight", &temp) != tinyxml2::XML_SUCCESS || temp > std::numeric_limits<
    unsigned short>::max()) {
    std::cerr << "Necessary element missing or invalid: tileheight" << std::endl;
    assert(false);
  }
  tile_height_ = static_cast<unsigned short>(temp);

  if (tileset->QueryUnsignedAttribute("columns", &temp) != tinyxml2::XML_SUCCESS || temp > std::numeric_limits<unsigned
    short>::max()) {
    std::cerr << "Necessary element missing or invalid: columns" << std::endl;
    assert(false);
  }
  columns_count_ = static_cast<unsigned short>(temp);

  auto* image = tileset->FirstChildElement("image");
  if (image) {
    if (image->QueryUnsignedAttribute("width", &temp) != tinyxml2::XML_SUCCESS || temp > std::numeric_limits<unsigned
      short>::max()) {
      std::cerr << "Necessary element missing or invalid: width" << std::endl;
      assert(false);
    }
    texture_width_ = static_cast<unsigned short>(temp);

    if (image->QueryUnsignedAttribute("height", &temp) != tinyxml2::XML_SUCCESS || temp > std::numeric_limits<unsigned
      short>::max()) {
      std::cerr << "Necessary element missing or invalid: height" << std::endl;
      assert(false);
    }
    texture_height_ = static_cast<unsigned short>(temp);
  }
  else {
    std::cerr << "Necessary element missing or invalid: image" << std::endl;
    assert(false);
  }

  // each tile specific data
  for (auto* tile = tileset->FirstChildElement("tile"); tile; tile = tile->NextSiblingElement("tile")) {
    unsigned int tileId;
    if (tile->QueryUnsignedAttribute("id", &tileId) != tinyxml2::XML_SUCCESS) {
      std::cerr << "Necessary tile element missing or invalid: id" << std::endl;
      assert(false);
    }

    // Animation data
    auto* animation = tile->FirstChildElement("animation");
    if (animation) {
      TileAnimationData animData;
      auto* properties = tile->FirstChildElement("properties");
      if (properties) {
        for (auto* prop = properties->FirstChildElement("property"); prop; prop = prop->
             NextSiblingElement("property")) {
          const char* propName = prop->Attribute("name");
          if (!propName) continue;
          if (std::strcmp(propName, "loop") == 0) {
            prop->QueryBoolAttribute("value", &animData.is_loop);
          }
          else if (std::strcmp(propName, "onstart") == 0) {
            prop->QueryBoolAttribute("value", &animData.play_on_start);
          }
        }
      }

      // Animation frame data
      for (auto* frame = animation->FirstChildElement("frame"); frame; frame = frame->NextSiblingElement("frame")) {
        unsigned int frameTileId;
        float duration;
        if (frame->QueryUnsignedAttribute("tileid", &frameTileId) != tinyxml2::XML_SUCCESS) {
          std::cerr << "Missing or invalid 'tileid' attribute in frame" << std::endl;
          assert(false);
        }
        if (frame->QueryFloatAttribute("duration", &duration) != tinyxml2::XML_SUCCESS) {
          std::cerr << "Missing or invalid 'duration' attribute in frame" << std::endl;
          assert(false);
        }
        animData.frames.push_back(GetUvById(frameTileId));
        animData.frame_durations.push_back(duration / 1000.0f); // normalize to seconds
      }

      if (!animData.frames.empty()) {
        tile_animated_data_[tileId] = animData;
      }
    }

    // Collision
    auto* objectgroup = tile->FirstChildElement("objectgroup");
    if (objectgroup) {
      std::vector<CollisionData> collisionData;
      for (auto* object = objectgroup->FirstChildElement("object"); object; object = object->
           NextSiblingElement("object")) {
        CollisionData data;
        if (object->QueryUnsignedAttribute("x", &data.x) != tinyxml2::XML_SUCCESS) {
          std::cerr << "Missing or invalid 'x' attribute in object" << std::endl;
          assert(false);
        }
        if (object->QueryUnsignedAttribute("y", &data.y) != tinyxml2::XML_SUCCESS) {
          std::cerr << "Missing or invalid 'y' attribute in object" << std::endl;
          assert(false);
        }
        if (object->QueryUnsignedAttribute("width", &data.width) != tinyxml2::XML_SUCCESS) {
          std::cerr << "Missing or invalid 'width' attribute in object" << std::endl;
          assert(false);
        }
        if (object->QueryUnsignedAttribute("height", &data.height) != tinyxml2::XML_SUCCESS) {
          std::cerr << "Missing or invalid 'height' attribute in object" << std::endl;
          assert(false);
        }
        data.is_circle = object->FirstChildElement("ellipse") != nullptr;
        collisionData.push_back(data);
      }
      if (!collisionData.empty()) {
        tile_collision_data_[tileId] = collisionData;
      }
    }
  }
}
