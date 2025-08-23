module;

module game.mobs_manager;

import game.mobs.slime;

void MobManager::Spawn(TileMapObjectProps props) {
  if (props.type == TileMapObjectType::MOB_SLIME) {
    [[maybe_unused]] auto id = object_pool_.Insert(mob::slime::MakeMobData(props));
  }
}
