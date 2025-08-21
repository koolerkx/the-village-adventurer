module;

export module game.scene_object;

import std;
import graphic.direct3D;
import game.types;
import graphic.utils.types;
import game.collision.collider;

export namespace scene_object {
  void LoadTexture(FixedPoolIndexType& id, const std::wstring_view texture_path, TextureManager* tm) {
    id = tm->Load(texture_path.data());
  }

  void Render(const RenderItem& render_item, Renderer* renderer) {
    renderer->DrawSprite(render_item.texture_id, render_item.transform, render_item.uv, render_item.color);
  }

  using AnimationState = TileAnimationState;
  using AnimationData = TileAnimationData;

  // static data used to hardcode the animation data
  struct AnimationFrameData {
    std::vector<TileUV> frames;
    std::vector<float> frame_durations;
  };

  template <unsigned int FramesPerLine, unsigned short FrameWidth, unsigned short FrameHeight, std::size_t... I>
  constexpr std::array<TileUV, sizeof...(I)> MakeFramesArray(std::index_sequence<I...>) {
    return {
      TileUV{
        .u = I % FramesPerLine * FrameWidth,
        .v = I / FramesPerLine * FrameHeight,
        .w = FrameWidth,
        .h = FrameHeight
      }...
    };
  }

  void UpdateAnimation(AnimationState& state, const float delta_time, UV& uv) {
    if (!state.is_playing || state.frames.empty()) return;

    state.current_frame_time += delta_time;
    if (state.current_frame_time < state.frame_durations[state.current_frame]) return;

    state.current_frame_time -= state.frame_durations[state.current_frame];
    state.current_frame++;

    if (state.current_frame >= state.frames.size()) {
      if (state.is_loop) {
        state.current_frame = 0;
      }
      else {
        state.current_frame = state.frames.size() - 1;
        state.is_playing = false;
      }
    }

    uv.position.x = static_cast<float>(state.frames[state.current_frame].u);
    uv.position.y = static_cast<float>(state.frames[state.current_frame].v);
  }

  constexpr std::vector<TileUV> MakeFramesVector(
    unsigned int frame_per_line,
    unsigned short frame_width,
    unsigned short frame_height,
    int total_frames,
    unsigned int offset_u = 0,
    unsigned int offset_v = 0
  ) {
    return std::views::iota(0, total_frames)
      | std::views::transform([=](int i) -> TileUV {
        return TileUV{
          .u = offset_u + (i % frame_per_line) * frame_width,
          .v = offset_v + (i / frame_per_line) * frame_height,
          .w = frame_width,
          .h = frame_height
        };
      })
      | std::ranges::to<std::vector>();
  }

  constexpr std::vector<float> MakeFramesConstantDuration(
    float duration_per_frame, int frame_count) {
    return std::views::iota(0, frame_count)
      | std::views::transform([=](int) -> float {
        return duration_per_frame;
      }) | std::ranges::to<std::vector>();
  }

  float GetPlayerRotationByDirection(Vector2 direction) {
    if (direction.x == 0.0f && direction.y == 0.0f)
      return 0.0f;

    return std::atan2(direction.y, direction.x);
  }

  Vector2 RotateAroundPivot(Vector2 p, Vector2 pivot, float rad) {
    float s = std::sin(rad), c = std::cos(rad);
    float ux = p.x - pivot.x, uy = p.y - pivot.y;
    return {ux * c - uy * s + pivot.x, ux * s + uy * c + pivot.y};
  }

  std::array<Vector2, 4> GetRotatedPoints(const RectCollider shape, Vector2 pivot, float rotation) {
    const std::array<Vector2, 4> corners = {
      Vector2{shape.x, shape.y},                              // left-top
      Vector2{shape.x + shape.width, shape.y},                // right-top
      Vector2{shape.x + shape.width, shape.y + shape.height}, // right-bottom
      Vector2{shape.x, shape.y + shape.height}                // left-bottom
    };

    // const float angleRad = rotationDeg * static_cast<float>(std::numbers::pi) / 180.0f;
    const float cosA = std::cos(rotation);
    const float sinA = std::sin(rotation);

    std::array<Vector2, 4> out{};
    for (size_t i = 0; i < corners.size(); ++i) {
      const float tx = corners[i].x - pivot.x;
      const float ty = corners[i].y - pivot.y;

      const float rx = tx * cosA - ty * sinA;
      const float ry = tx * sinA + ty * cosA;

      out[i] = {rx + pivot.x, ry + pivot.y};
    }
    return out;
  }
}
