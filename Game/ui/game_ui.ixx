module;

export module game.ui.game_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;

export class GameUI {
private:
  FixedPoolIndexType texture_id_;

public:
  GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path);
  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
};
