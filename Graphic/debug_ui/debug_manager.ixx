module;
#include <Windows.h>

export module graphic.debug;

import std;
import graphic.direct3D;
import graphic.utils.font;
import graphic.utils.types;

struct DebugSetting {
  bool is_show_fps = true;
  bool is_show_frame_delta = true;

  bool is_show_ui_gridline = true;
  bool is_show_ui_column = true;
  bool is_show_ui_row = true;
};

struct TextSpriteProps {
  Transform transform;
  std::wstring font_key;
  StringSpriteProps font_props;
};

export struct UiGuidelineProps {
  int grid_size = 16;
  COLOR grid_color = color::setOpacity(color::red, 0.1f);

  int column_count = 8;
  int column_margin = 24;
  int column_gutter = 24;
  COLOR column_color = color::setOpacity(color::red, 0.1f);

  int row_count = 4;
  int row_margin = 0;
  int row_gutter = 16;
  COLOR row_color = color::setOpacity(color::red, 0.1f);

  TextSpriteProps debug_text_props = {
    .transform = Transform{
      .position = {16.0f, 16.0f, 0.0f},
      .size = {12.0f, 12.0f},
      .scale = {1.0f, 1.0f},
    },
    .font_key = L"",
    .font_props = {
      .pixel_size = 24.0f,
      .letter_spacing = 0.0f,
      .line_height = 0.0f
    },
  };
};

export struct DebugContext {
  const ResourceManager* render_resource_manager = nullptr;
  SIZE window_size = {0, 0};
};

export class DebugManager {
private:
  DebugSetting debug_setting_;
  UiGuidelineProps props_;
  std::unique_ptr<DebugContext> context_;

  std::vector<Line> grid_lines_;
  std::vector<Rect> rects_;

  std::wstring font_key_;
  FixedPoolIndexType font_texture_id_;
  
  float fps_smooth_ = 0.0f;
  float fixed_fps_smooth_ = 0.0f;
  float delta_time_ = 0.0f;
  
public:
  DebugManager(std::unique_ptr<DebugContext> context);

  void OnUpdate(float delta_time);
  void OnFixedUpdate(float delta_time);
  void OnRender();
};
