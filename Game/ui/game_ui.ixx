module;

export module game.ui.game_ui;

import std;
import game.context;
import game.scene_game.context;
import game.scene_object.camera;
import graphic.utils.fixed_pool;
import graphic.utils.font;
import graphic.utils.types;

std::unordered_map<std::string, UV> texture_map = {
  // HP Bar
  {"HPBarPanel", UV{{384, 0}, {160, 41}}},
  {"BigCircle", UV{{384, 41}, {41, 41}}},
  {"Heart", UV{{508, 41}, {32, 32}}},
  {"HPBar", UV{{224, 128}, {120, 8}}},
  {"HPBarFrame", UV{{224, 112}, {128, 16}}},

};
export class GameUI {
private:
  FixedPoolIndexType texture_id_;
  std::wstring font_key_;

  Font* default_font_;
public:
  GameUI(GameContext* ctx, SceneContext* scene_ctx, std::wstring texture_path);
  void OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time);
  void OnRender(GameContext* ctx, SceneContext* scene_ctx, Camera* camera);
};
