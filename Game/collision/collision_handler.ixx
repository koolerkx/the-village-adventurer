export module game.collision_handler;

import std;
import game.collision.collider;
import game.types;
import game.scene_object;

#pragma region VECTOR_HELPER_FUNCTION
// Vector Calculation
float dot(const Vector2& a, const Vector2& b) { return a.x * b.x + a.y * b.y; }

float length(const Vector2& v) { return std::sqrt(dot(v, v)); }

Vector2 normalize(const Vector2& v) {
  float len = length(v);
  return (len > 1e-7f) ? Vector2{v.x / len, v.y / len} : Vector2{0, 0};
}

struct RectOBB {
  Vector2 p[4];   // world-space corners: 0=LT, 1=RT, 2=RB, 3=LB
  Vector2 center; // center
  Vector2 u;      // x-axis direction  : p1 - p0
  Vector2 v;      // y-axis direction  : p3 - p0
  float hx;       // half width  = 0.5 * |p1 - p0|
  float hy;       // half height = 0.5 * |p3 - p0|
};

template <typename Owner>
RectOBB MakeRectOBB(const Collider<Owner>& collider) {
  const auto& shape = std::get<RectCollider>(collider.shape);

  // Do Rotation
  std::array<Vector2, 4> local = scene_object::GetRotatedPoints(
    {shape.x, shape.y, shape.width, shape.height},
    {
      shape.base_width * 0.5f + collider.rotation_pivot.x,
      shape.base_height * 0.5f + collider.rotation_pivot.y
    },
    collider.rotation);

  RectOBB obb{};

  // World Coordinate
  for (int i = 0; i < 4; ++i) {
    obb.p[i] = {
      local[i].x + collider.position.x,
      local[i].y + collider.position.y
    };
  }

  Vector2 e0 = {obb.p[1].x - obb.p[0].x, obb.p[1].y - obb.p[0].y}; // LT->RT
  Vector2 e3 = {obb.p[3].x - obb.p[0].x, obb.p[3].y - obb.p[0].y}; // LT->LB

  obb.hx = 0.5f * length(e0);
  obb.hy = 0.5f * length(e3);
  obb.u = normalize(e0);
  obb.v = normalize(e3);

  obb.center = {(obb.p[0].x + obb.p[2].x) * 0.5f, (obb.p[0].y + obb.p[2].y) * 0.5f};

  return obb;
}

auto absdot = [](const Vector2& x, const Vector2& y) {
  return std::fabs(dot(x, y));
};
#pragma endregion


template <typename A, typename B>
bool CollideRectRectOBB(const Collider<A>& aCol, const Collider<B>& bCol) {
  const RectOBB ARect = MakeRectOBB(aCol);
  const RectOBB BRect = MakeRectOBB(bCol);

  const Vector2 t{BRect.center.x - ARect.center.x, BRect.center.y - ARect.center.y};

  auto separated_on = [&](const Vector2& L, float rA, float rB) {
    const float dist = std::fabs(dot(t, L));
    return dist > (rA + rB);
  };

  // axis A.u
  {
    const float rA = ARect.hx;
    const float rB = BRect.hx * absdot(BRect.u, ARect.u) + BRect.hy * absdot(BRect.v, ARect.u);
    if (separated_on(ARect.u, rA, rB)) return false;
  }

  // axis A.v
  {
    const float rA = ARect.hy;
    const float rB = BRect.hx * absdot(BRect.u, ARect.v) + BRect.hy * absdot(BRect.v, ARect.v);
    if (separated_on(ARect.v, rA, rB)) return false;
  }

  // axis B.u
  {
    const float rA = ARect.hx * absdot(ARect.u, BRect.u) + ARect.hy * absdot(ARect.v, BRect.u);
    const float rB = BRect.hx;
    if (separated_on(BRect.u, rA, rB)) return false;
  }

  // axis B.v
  {
    const float rA = ARect.hx * absdot(ARect.u, BRect.v) + ARect.hy * absdot(ARect.v, BRect.v);
    const float rB = BRect.hy;
    if (separated_on(BRect.v, rA, rB)) return false;
  }

  return true;
}


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

  // TODO: handle MTV
  template <typename A, typename B>
  bool CollideCircleRect(const Collider<A>& circleCol, const Collider<B>& rectCol) {
    const RectOBB obb = MakeRectOBB(rectCol);

    const auto& shape = std::get<CircleCollider>(circleCol.shape);
    const Vector2 circleCenter{
      circleCol.position.x + shape.x,
      circleCol.position.y + shape.y
    };
    const float radius = shape.radius;

    // vector of two center, projected to obb axis
    const Vector2 d{circleCenter.x - obb.center.x, circleCenter.y - obb.center.y};
    const float dxu = dot(d, obb.u);
    const float dyv = dot(d, obb.v);

    const float qx = std::clamp(dxu, -obb.hx, obb.hx);
    const float qy = std::clamp(dyv, -obb.hy, obb.hy);

    const Vector2 closest{
      obb.center.x + obb.u.x * qx + obb.v.x * qy,
      obb.center.y + obb.u.y * qx + obb.v.y * qy
    };

    // closest to circle center
    Vector2 n{circleCenter.x - closest.x, circleCenter.y - closest.y};
    const float dist2 = dot(n, n);
    const float r2 = radius * radius;

    return dist2 <= r2;
  }

  template <typename A, typename B>
  void HandleDetection(
    Collider<A> a,
    std::span<Collider<B>> b_pool,
    const std::invocable<A*, B*, CollisionResult> auto& on_trigger
  ) {
    if (std::holds_alternative<CircleCollider>(a.shape)) {
      for (auto& b : b_pool) {
        if (!std::holds_alternative<RectCollider>(b.shape)) continue;

        if (CollideCircleRect(a, b)) {
          // TODO: handle MTV
          on_trigger(a.owner, b.owner, {true});
        }
      }
    }
    if (std::holds_alternative<RectCollider>(a.shape)) {
      for (auto& b : b_pool) {
        if (std::holds_alternative<CircleCollider>(b.shape)) {
          if (CollideCircleRect(b, a)) {
            on_trigger(a.owner, b.owner, {true});
          }
        }
        if (std::holds_alternative<RectCollider>(b.shape)) {
          CollisionResult result = CollideAABB(a, b);

          if (result.is_colliding) {
            on_trigger(a.owner, b.owner, result);
          }
        }
      }
    }
  }

  // TODO: handle MTV 
  template <typename A, typename B>
  void HandleDetection(
    std::span<Collider<A>> a_pool,
    std::span<Collider<B>> b_pool,
    const std::invocable<A*, B*, CollisionResult> auto& on_trigger
  ) {
    for (auto& a : a_pool) {
      for (auto& b : b_pool) {
        bool is_collide = false;
        if (std::holds_alternative<RectCollider>(a.shape) && std::holds_alternative<RectCollider>(b.shape)) {
          is_collide = CollideRectRectOBB(a, b);
        }
        else if (std::holds_alternative<CircleCollider>(a.shape) && std::holds_alternative<RectCollider>(b.shape)) {
          is_collide = CollideCircleRect(a, b);
        }

        if (!is_collide) continue;

        on_trigger(a.owner, b.owner, {true});
      }
    }
  }
}
