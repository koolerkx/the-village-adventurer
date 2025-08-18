module;

export module game.scene_game;

import std;
import game.scene;
import game.map;

import game.scene_object.player;

export class GameScene : public IScene {
private:
  std::unique_ptr<TileMap> map_ {nullptr};

  std::unique_ptr<Player> player_ = nullptr;
  
public:
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
