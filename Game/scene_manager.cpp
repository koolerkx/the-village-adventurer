module;

module game.scene_manager;

import std;

SceneManager::SceneManager(
  std::unique_ptr<IScene> initial_scene,
  std::unique_ptr<GameContext> game_context,
  std::unique_ptr<GameConfig> game_config
) {
  game_context_ = std::move(game_context);
  game_config_ = std::move(game_config);

  // Load Map
  tile_repository_ = std::make_unique<TileRepository>(game_config_->map_tile_filepath);
  
  ChangeScene(std::move(initial_scene));
}

void SceneManager::ChangeScene(std::unique_ptr<IScene> new_scene) {
  if (current_scene_) {
    current_scene_->OnExit(game_context_.get());
  }
  current_scene_ = std::move(new_scene);
  current_scene_->OnEnter(game_context_.get());
}

void SceneManager::OnUpdate(float delta_time) const {
  current_scene_->OnUpdate(game_context_.get(), delta_time);
}

void SceneManager::OnRender() const {
  current_scene_->OnRender(game_context_.get());
}
