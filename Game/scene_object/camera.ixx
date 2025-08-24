module;

export module game.scene_object.camera;

import std;
import graphic.direct3D;
import graphic.utils.types;
import game.types;
import game.context;

export class Camera {
private:
  Vector2 position_ = {0.0f, 0.0f};
  float zoom_ = 3.0f;

  Vector2 velocity_ = {0.0f, 0.0f};

public:
  Vector2 GetPosition() const { return position_; }
  float GetZoom() const { return zoom_; }

  void SetPosition(Vector2 position) { position_ = position; }
  void SetPosition(float x, float y) { position_ = {x, y}; }
  void SetPosition(const std::function<Vector2(Camera*)>& fn) { position_ = fn(this); }
  void SetZoom(float zoom) { zoom_ = zoom; }

  void UpdatePosition(const Vector2& target_position, float delta_time) {
    static constexpr float follow_speed = 7.5f; // maximum center offset

    position_ = {
      std::lerp(position_.x, target_position.x, follow_speed * delta_time),
      std::lerp(position_.y, target_position.y, follow_speed * delta_time)
    };
  }

  CameraProps GetCameraProps() const { return {{position_.x, position_.y, 0.0f}, zoom_, AlginPivot::CENTER_CENTER}; }
};
