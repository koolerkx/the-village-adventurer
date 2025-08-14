module;

export module game.scene_manager;

import std;
import graphic.direct3D;
export import game.scene;
export import game.context;

import game.map.tile_repository;

export struct GameConfig {
  std::string default_map;
  std::string map_texture_filepath;
  std::string map_tile_filepath;
};

export class SceneManager {
private:
  inline static std::unique_ptr<GameContext> game_context_{};
  inline static std::unique_ptr<GameConfig> game_config_{};

  inline static std::unique_ptr<IScene> current_scene_{};
  inline static std::unique_ptr<IScene> pending_scene_{};

  // Map Related
  std::unique_ptr<TileRepository> tile_repository_{};

  bool is_scene_change_pending_ = false;
  SceneManager() = default;

public:
  static SceneManager& Init(std::unique_ptr<IScene> initial_scene,
                            std::unique_ptr<GameContext> game_context,
                            std::unique_ptr<GameConfig> game_config
  );

  static SceneManager& GetInstance() {
    static SceneManager instance;
    return instance;
  };

  GameConfig* GetGameConfig() const { return game_config_.get(); };

  void SetTileRepository(std::unique_ptr<TileRepository> tr) { tile_repository_ = std::move(tr); };
  TileRepository* GetTileRepository() const { return tile_repository_.get(); };

  void ChangeScene(std::unique_ptr<IScene> new_scene);
  void ChangeSceneDelayed(std::unique_ptr<IScene> new_scene);
  void ProcessPendingSceneChange();

  void OnUpdate(float delta_time);
  void OnFixedUpdate(float delta_time) const;
  void OnRender() const;
};
