module;

module game.scene_game;

import std;
import graphic.utils.types;

void GameScene::OnEnter(GameContext* ctx) {
  std::cout << "GameScene> OnEnter" << std::endl;
}

void GameScene::OnUpdate(GameContext*, float delta_time) {
  // std::cout << "GameScene> OnUpdate" << std::endl;
}

void GameScene::OnRender(GameContext* ctx) {
  // std::cout << "GameScene> OnRender" << std::endl;
}

void GameScene::OnExit(GameContext*) {
  std::cout << "GameScene> OnExit" << std::endl;
}
