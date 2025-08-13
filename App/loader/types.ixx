/**
 * @file types.ixx
 * @brief yaml config schema, storing default value
 * @author Kooler Fan
 * @date 2025-08-13
 */

module;

export module app.loader.types;

import std;

export struct ShaderFiles {
  std::string vertex_shader{"assets/shaders/shader_vertex_2d.cso"};
  std::string instanced_vertex_shader{"assets/shaders/instanced_shader_vertex_2d.cso"};
  std::string pixel_shader{"assets/shaders/shader_pixel_2d.cso"};
};

export struct GraphicConfig {
  bool horizontal_sync{true};
  int window_size_width{1280};
  int window_size_height{720};
  ShaderFiles shader_files{};
  bool handle_dps{false};
};

export struct MapConfig {
  std::string default_map;
  std::string map_metadata;
  std::vector<std::string> files;
  std::vector<std::string> animate_files;
};

export struct Config {
  GraphicConfig graphic;
  MapConfig map;
};