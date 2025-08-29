export module game.scene_game.context;

import std;
import game.map.linked_map;
import game.scene_object.skill;

export struct SceneContext {
  std::shared_ptr<LinkedMapNode> active_map_node = nullptr;
  SkillManager* skill_manager = nullptr;
};
