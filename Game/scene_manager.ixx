module;

export module game.scene_manager;

import std;
import graphic.direct3D;
export import game.scene;
export import game.context;

import game.map.tile_repository;

export struct GameConfig {
  std::string default_map;
  std::string map_tile_filepath;
};

export class SceneManager {
private:
  std::unique_ptr<IScene> current_scene_;
  std::unique_ptr<GameContext> game_context_;
  std::unique_ptr<GameConfig> game_config_;

  // Map Related
  std::unique_ptr<TileRepository> tile_repository_;

public:
  SceneManager(
    std::unique_ptr<IScene> initial_scene,
    std::unique_ptr<GameContext> game_context,
    std::unique_ptr<GameConfig> game_config_
  );

  void ChangeScene(std::unique_ptr<IScene> new_scene);

  void OnUpdate(float delta_time) const;
  void OnRender() const;
};
