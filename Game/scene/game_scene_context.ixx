export module game.scene_game.context;

import game.map;
import game.scene_object.skill;

export struct SceneContext {
  TileMap* map = nullptr;
  SkillManager* skill_manager = nullptr;
};
