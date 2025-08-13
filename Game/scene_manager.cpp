module;

module game.scene_manager;

import std;

SceneManager& SceneManager::Init(std::unique_ptr<IScene> initial_scene,
                                 std::unique_ptr<GameContext> game_context,
                                 std::unique_ptr<GameConfig> game_config) {
  game_context_ = std::move(game_context);
  game_config_ = std::move(game_config);


  // Scene Manager Setup
  SceneManager& sm = GetInstance();
  // Load Map
  sm.SetTileRepository(std::make_unique<TileRepository>(game_config_->map_tile_filepath));

  // Enter initial Scene
  sm.ChangeScene(std::move(initial_scene));

  return sm;
}


void SceneManager::ChangeScene(std::unique_ptr<IScene> new_scene) {
  if (current_scene_) {
    current_scene_->OnExit(game_context_.get());
  }
  current_scene_ = std::move(new_scene);
  current_scene_->OnEnter(game_context_.get());
  is_scene_change_pending_ = false;
}

void SceneManager::ChangeSceneDelayed(std::unique_ptr<IScene> new_scene) {
  pending_scene_ = std::move(new_scene);
  is_scene_change_pending_ = true;
}

void SceneManager::ProcessPendingSceneChange() {
  if (is_scene_change_pending_) {
    if (current_scene_) {
      current_scene_->OnExit(game_context_.get());
    }
    current_scene_ = std::move(pending_scene_);
    is_scene_change_pending_ = false;
    current_scene_->OnEnter(game_context_.get());
  }
}

void SceneManager::OnUpdate(float delta_time) {
  if (current_scene_ && !is_scene_change_pending_) {
    current_scene_->OnUpdate(game_context_.get(), delta_time);
  }
  ProcessPendingSceneChange();
}

void SceneManager::OnFixedUpdate(float delta_time) const {}

void SceneManager::OnRender() const {
  current_scene_->OnRender(game_context_.get());
}
