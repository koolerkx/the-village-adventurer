module;

module graphic.debug;

import std;

DebugManager::DebugManager(std::unique_ptr<DebugContext> context) {
  context_ = std::move(context);
  
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

void DebugManager::OnUpdate(const float delta_time) {}

void DebugManager::OnRender() {
  if (debug_setting_.is_show_ui_gridline) {
    context_->render_resource_manager->renderer->DrawLinesForDebugUse(grid_lines_);
  }
}
