module;

export module game.math;

import std;
import game.types;
import graphic.utils.types;

export namespace math {
  /**
   * @brief calculate the normalized direction from a to b
   * @param a origin
   * @param b target
   * @return normalized direction from a to b 
   */
  Vector2 GetDirection(Vector2 a, Vector2 b) {
    const float dir_x = b.x - a.x;
    const float dir_y = b.y - a.y;

    float magnitude = std::sqrt(dir_x * dir_x + dir_y * dir_y);

    if (magnitude < std::numeric_limits<float>::epsilon()) {
      return {0.0f, 0.0f};
    }

    const float inv_mag = 1.0f / magnitude;
    return {dir_x * inv_mag, dir_y * inv_mag};
  }

  /**
   * @brief calculate the normalized direction from a to b
   * @param a origin
   * @param b target
   * @return normalized direction from a to b 
   */
  Vector2 GetDirection(POSITION a, POSITION b) {
    return GetDirection({a.x, a.y}, {b.x, b.y});
  }

  float GetDistance(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return dx * dx + dy * dy;
  }

  /**
   * @ref https://easings.net/
   */
  namespace interpolation {
    float EaseInOutQuint(float x) {
      return (x < 0.5f)
               ? 16.0f * x * x * x * x * x
               : 1.0f - static_cast<float>(std::pow(-2.0f * x + 2.0f, 5)) / 2.0f;
    }
  }
};
