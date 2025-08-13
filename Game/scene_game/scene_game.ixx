module;

export module game.scene_game;

import game.scene;

export class GameScene : public IScene {
private:
  FixedPoolIndexType texture_id;
  
public:
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
