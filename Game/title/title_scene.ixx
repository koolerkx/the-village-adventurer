module;

export module game.title_scene;

import game.scene;

export class TitleScene : public IScene {
private:
  FixedPoolIndexType texture_id;
  
public:
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
