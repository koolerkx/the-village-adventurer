module;

export module game.map.linked_map;

import std;
import game.map;

export struct LinkedMapNode {
  int x, y;
  std::weak_ptr<TileMap> data{};
  std::weak_ptr<LinkedMapNode> up{};
  std::weak_ptr<LinkedMapNode> down{};
  std::weak_ptr<LinkedMapNode> left{};
  std::weak_ptr<LinkedMapNode> right{};
};
