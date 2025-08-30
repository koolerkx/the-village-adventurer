module;

export module game.context;

export import game.input;
export import graphic.direct3D;

export struct GameContext {
  const ResourceManager* render_resource_manager = nullptr;
  InputHandler* input_handler = nullptr;
  int window_width = 0;
  int window_height = 0;
  bool allow_control = true;
};