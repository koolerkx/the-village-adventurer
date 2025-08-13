module;

export module graphic.utils.config;

import std;

export struct Dx11WrapperConfig {
  bool horizontal_sync{true};
  int window_size_width{1280};
  int window_size_height{720};
  std::string vertex_shader{"assets/shaders/shader_vertex_2d.cso"};
  std::string instanced_vertex_shader{"assets/shaders/instanced_shader_vertex_2d.cso"};
  std::string pixel_shader{"assets/shaders/shader_pixel_2d.cso"};
};