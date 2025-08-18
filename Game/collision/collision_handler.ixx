export module game.collision_handler;

import std;
import game.collision.collider;
import game.types;

export namespace collision {
  struct CollisionResult {
    bool is_colliding;
    Vector2 mtv;
    float overlapX = 0.0f;
    float overlapY = 0.0f;
  };

  template <typename A, typename B>
  CollisionResult CollideAABB(const Collider<A>& a, const Collider<B>& b) {
    const RectCollider& rectA = std::get<RectCollider>(a.shape);
    const RectCollider& rectB = std::get<RectCollider>(b.shape);

    // AABB world coordinates
    float ax1 = a.position.x + rectA.x;
    float ay1 = a.position.y + rectA.y;
    float ax2 = ax1 + rectA.width;
    float ay2 = ay1 + rectA.height;

    float bx1 = b.position.x + rectB.x;
    float by1 = b.position.y + rectB.y;
    float bx2 = bx1 + rectB.width;
    float by2 = by1 + rectB.height;

    const bool is_collided = ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;

    if (!is_collided) return {false, Vector2{0, 0}};

    float overlapX = std::min(ax2, bx2) - std::max(ax1, bx1);
    float overlapY = std::min(ay2, by2) - std::max(ay1, by1);

    Vector2 mtv;
    float dir_x = (a.position.x < b.position.x) ? -1.0f : 1.0f;
    float dir_y = (a.position.y < b.position.y) ? -1.0f : 1.0f;
    if (overlapX < overlapY) {
      mtv = Vector2{overlapX * dir_x, 0.0f};
    }
    else {
      mtv = Vector2{0.0f, overlapY * dir_y};
    }

    return {true, mtv, overlapX * dir_x, overlapY * dir_y};
  }

  template <typename A, typename B>
  void HandleDetection(
    Collider<A> a,
    std::span<Collider<B>> b_pool,
    const std::invocable<A*, B*, CollisionResult> auto& on_trigger
  ) {
    if (!std::holds_alternative<RectCollider>(a.shape)) return;

    for (auto& b : b_pool) {
      if (!std::holds_alternative<RectCollider>(b.shape)) continue;

      CollisionResult result = CollideAABB(a, b);

      if (result.is_colliding) {
        on_trigger(a.owner, b.owner, result);
      }
    }
  }
}
