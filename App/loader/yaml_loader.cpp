module;
#define YAML_CPP_STATIC_DEFINE
#include "yaml-cpp/yaml.h"
#include <cassert>

module app.loader.yaml;

YAMLConfigLoader::YAMLConfigLoader(const std::string& config_path) {
  loadConfig(config_path);
}

const Config& YAMLConfigLoader::getConfig() const noexcept {
  return config_;
}

void YAMLConfigLoader::loadConfig(const std::string& config_path) {
  try {
    if (!std::filesystem::exists(config_path)) {
      std::cerr << "Config file not found: " + config_path << std::endl;
      throw std::runtime_error("Config file not found: " + config_path);
    }

    YAML::Node config = YAML::LoadFile(config_path);

    if (auto graphic_node = config["config"]["graphic"]) {
      config_.graphic.horizontal_sync =
        graphic_node["horizontal_sync"].as<bool>(config_.graphic.horizontal_sync);
      config_.graphic.window_size_width =
        graphic_node["window_size_width"].as<int>(config_.graphic.window_size_width);
      config_.graphic.window_size_height =
        graphic_node["window_size_height"].as<int>(config_.graphic.window_size_height);
      config_.graphic.handle_dps =
        graphic_node["handle_dps"].as<bool>(config_.graphic.handle_dps);

      if (auto shader_node = graphic_node["shader_files"]) {
        config_.graphic.shader_files.vertex_shader =
          shader_node["vertex_shader"].as<std::string>(config_.graphic.shader_files.vertex_shader);
        config_.graphic.shader_files.instanced_vertex_shader =
          shader_node["instanced_vertex_shader"].as<
            std::string>(config_.graphic.shader_files.instanced_vertex_shader);
        config_.graphic.shader_files.pixel_shader =
          shader_node["pixel_shader"].as<std::string>(config_.graphic.shader_files.pixel_shader);
      }
    }

    if (auto map_node = config["config"]["map"]) {
      config_.map.default_map =
        map_node["default_map"].as<std::string>(config_.map.default_map);
      config_.map.map_metadata =
        map_node["map_metadata"].as<std::string>(config_.map.map_metadata);

      if (auto files_node = map_node["map"]["files"]) {
        config_.map.files = files_node.as<std::vector<std::string>>();
      }
      if (auto animate_node = map_node["map"]["animate_files"]) {
        config_.map.animate_files = animate_node.as<std::vector<std::string>>();
      }
    }
  }
  catch (const YAML::Exception& e) {
    std::cerr << "Failed to parse config file: " + std::string(e.what()) << std::endl;
    assert(false);
  }
  catch (const std::exception& e) {
    std::cerr << "Error loading config: " + std::string(e.what()) << std::endl;
    assert(false);
  }
}
