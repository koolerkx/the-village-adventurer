module;

module graphic.debug;

import std;

DebugManager::DebugManager(std::unique_ptr<DebugContext> context) {
  context_ = std::move(context);

  if (debug_setting_.is_show_ui_gridline) {
    for (int y = 0; y <= context_->window_size.cy; y += props_.grid_size) {
      float draw_y = static_cast<float>(y);

      grid_lines_.push_back({
        {0, draw_y, 0},
        {static_cast<float>(context_->window_size.cx), draw_y, 0},
        props_.grid_color
      });
    }

    for (int x = 0; x <= context_->window_size.cx; x += props_.grid_size) {
      float draw_x = static_cast<float>(x);
      grid_lines_.push_back({
        {draw_x, 0, 0},
        {draw_x, static_cast<float>(context_->window_size.cy), 0},
        props_.grid_color
      });
    }
  }
  if (debug_setting_.is_show_ui_column) {
    int column_width = (context_->window_size.cx - props_.column_margin * 2
      - props_.column_gutter * (props_.column_count - 1)) / props_.column_count;

    auto columns = std::views::iota(0, props_.column_count) |
      std::views::transform([&](int i) {
        float start_x = props_.column_margin + i * (column_width + props_.column_gutter);
        float end_x = start_x + column_width;

        return Rect{
          {start_x, 0.0f, 0.0f},
          {end_x, static_cast<float>(context_->window_size.cy), 0.0f},
          props_.column_color
        };
      });
    rects_.insert(rects_.end(), columns.begin(), columns.end());
  }
  if (debug_setting_.is_show_ui_row) {
    int row_height = (context_->window_size.cy - props_.row_margin * 2
      - props_.row_gutter * (props_.row_count - 1)) / props_.row_count;

    auto rows = std::views::iota(0, props_.row_count) |
      std::views::transform([&](int i) {
        float start_y = props_.row_margin + i * (row_height + props_.row_gutter);
        float end_y = start_y + row_height;

        return Rect{
          {0.0f, start_y, 0.0f},
          {static_cast<float>(context_->window_size.cx), end_y, 0.0f},
          props_.row_color
        };
      });

    rects_.insert(rects_.end(), rows.begin(), rows.end());
  }
}

void DebugManager::OnUpdate(const float delta_time) {
  static float fps_smooth = 0.0f;
  const float alpha = 0.1f; // 平滑係數，0.0 ~ 1.0
  float current_fps = (delta_time > 0.0f) ? (1.0f / delta_time) : 0.0f;
  fps_smooth = fps_smooth * (1.0f - alpha) + current_fps * alpha;
  std::println("FPS: {:.2f}", fps_smooth);
}

void DebugManager::OnRender() {
  if (debug_setting_.is_show_ui_gridline) {
    context_->render_resource_manager->renderer->DrawLinesForDebugUse(grid_lines_);
  }

  context_->render_resource_manager->renderer->DrawRectsForDebugUse(rects_);
}
