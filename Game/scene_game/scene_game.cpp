module;

module game.scene_game;

import std;
import game.scene_manager;
import graphic.utils.types;
import game.map.tile_repository;

void GameScene::OnEnter(GameContext* ctx) {
  std::cout << "GameScene> OnEnter" << std::endl;

  std::string default_map = SceneManager::GetInstance().GetGameConfig()->default_map;
  TileRepository* tr = SceneManager::GetInstance().GetTileRepository();

  map_.reset(new TileMap());
  std::string default_map_path = "map/map_data/" + default_map + ".tmx";

  map_->Load(default_map_path, tr);
}

void GameScene::OnUpdate(GameContext* ctx, float delta_time) {
  // std::cout << "GameScene> OnUpdate" << std::endl;

  map_->OnUpdate(ctx, delta_time);
}

void GameScene::OnRender(GameContext* ctx) {
  // std::cout << "GameScene> OnRender" << std::endl;

  

  map_->OnRender(ctx);
}

void GameScene::OnExit(GameContext*) {
  std::cout << "GameScene> OnExit" << std::endl;
}
