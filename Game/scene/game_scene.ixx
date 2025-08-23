module;

export module game.scene_game;

import std;
import game.scene;
import game.map;

import game.scene_object.player;
import game.scene_object.camera;
import game.scene_game.context;
import game.scene_object.skill;
import game.ui.game_ui;
import game.mobs_manager;

export class GameScene : public IScene {
private:
  std::unique_ptr<TileMap> map_{nullptr};

  std::unique_ptr<MobManager> mob_manager_ = nullptr;
  std::unique_ptr<SkillManager> skill_manager_ = nullptr;

  std::unique_ptr<Player> player_ = nullptr;
  std::unique_ptr<Camera> camera_ = nullptr;
  std::unique_ptr<GameUI> ui_ = nullptr;

  std::unique_ptr<SceneContext> scene_context = nullptr;

  std::chrono::time_point<std::chrono::steady_clock> time_at_start_;

  void HandlePlayerMovementAndCollisions(float delta_time);

  void ResetTimer();

public:
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
