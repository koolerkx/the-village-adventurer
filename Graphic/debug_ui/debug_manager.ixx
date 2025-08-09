module;
#include <Windows.h>

export module graphic.debug;

import std;
import graphic.direct3D;

export struct DebugSetting {
  bool is_show_fps = true;
  bool is_show_frame_delta = true;

  bool is_show_ui_gridline = true;
  bool is_show_ui_column = true;
  bool is_show_ui_row = true;
};

export struct UiGuidelineProps {
  int grid_size = 16;
  COLOR grid_color = color::AliceBlue;
  
  int column_count = 8;
  int column_margin = 24;
  int column_gutter = 24;

  int row_count = 4;
  int row_margin = 0;
  int row_gutter = 20;
};

export struct DebugContext {
  const ResourceManager* render_resource_manager = nullptr;
  SIZE window_size = {0, 0};
};

export class DebugManager {
private:
  static constexpr DebugSetting debug_setting_;
  static constexpr UiGuidelineProps guideline_props_;
  std::unique_ptr<DebugContext> context_;

public:
  DebugManager(std::unique_ptr<DebugContext> context);

  void OnUpdate(float delta_time);
  void OnRender();
};
