module;

module graphic.debug;

import std;

DebugManager::DebugManager(std::unique_ptr<DebugContext> context) {
  context_ = std::move(context);
}

void DebugManager::OnUpdate(const float delta_time) {}

void DebugManager::OnRender() {
  if (debug_setting_.is_show_ui_gridline) {
    for (int y = 0; y <= context_->window_size.cy; y += guideline_props_.grid_size) {
      float draw_y = static_cast<float>(y);

      context_->render_resource_manager->renderer->DrawLineForDebugUse(
        {0, draw_y, 0},
        {static_cast<float>(context_->window_size.cx), draw_y, 0},
        color::setOpacity(color::Red, 0.1)
      );
    }

    for (int x = 0; x <= context_->window_size.cy; x += guideline_props_.grid_size) {
      float draw_x = static_cast<float>(x);

      context_->render_resource_manager->renderer->DrawLineForDebugUse(
        {draw_x, 0, 0},
        {draw_x, static_cast<float>(context_->window_size.cy), 0},
        color::setOpacity(color::Red, 0.1)
      );
    }
  }
}
