module;

module graphic.debug;

import std;

DebugManager::DebugManager(std::unique_ptr<DebugContext> context) {
  context_ = std::move(context);
}

void DebugManager::OnUpdate(const float delta_time) {
}

void DebugManager::OnRender() {}
