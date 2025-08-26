module;

export module game.map.linked_map;

import std;
import game.map;

export struct LinkedMap {
  std::weak_ptr<TileMap> current{};
  std::weak_ptr<TileMap> top{};
  std::weak_ptr<TileMap> bottom{};
  std::weak_ptr<TileMap> left{};
  std::weak_ptr<TileMap> right{};
};
