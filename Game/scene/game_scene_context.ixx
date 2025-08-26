export module game.scene_game.context;

import std;
import game.map;
import game.scene_object.skill;

export struct SceneContext {
  std::shared_ptr<TileMap> map = nullptr;
  SkillManager* skill_manager = nullptr;
};
