module;

export module game.scene_manager;

import std;
import graphic.direct3D;
export import game.scene;
export import game.context;

export class SceneManager {
private:
  std::unique_ptr<IScene> current_scene_;
  std::unique_ptr<GameContext> game_context_;

public:
  SceneManager(
    std::unique_ptr<IScene> initial_scene,
    std::unique_ptr<GameContext> game_context
  );

  void ChangeScene(std::unique_ptr<IScene> new_scene);

  void OnUpdate(float delta_time) const;
  void OnRender() const;
};
