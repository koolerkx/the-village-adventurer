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

export namespace LinkedMap {
  std::vector<std::weak_ptr<LinkedMapNode>> MakeActiveMapNodes(std::shared_ptr<LinkedMapNode> node) {
    std::vector<std::weak_ptr<LinkedMapNode>> v;

    v.push_back(node->up);
    v.push_back(node->down);
    v.push_back(node->left);
    v.push_back(node->right);

    auto up = node->up.lock();
    v.push_back(up->left);
    v.push_back(up->right);
    auto down = node->down.lock();
    v.push_back(down->left);
    v.push_back(down->right);

    return v;
  }
}
