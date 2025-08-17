module;

module game.scene_object.player;

import std;

// Texture data
static constexpr std::wstring_view texture_path = L"assets/character_01.png"; // TODO: extract

Player::Player(GameContext* ctx) {
  const auto tm = ctx->render_resource_manager->texture_manager.get();

  texture_id_ = tm->Load(texture_path.data());
}

void Player::OnUpdate(GameContext* ctx, float delta_time) {
  // Handle Input
  direction_ = {0, 0};
  if (ctx->input_handler->GetKey(KeyCode::KK_W)) direction_.y -= 1.0f;
  if (ctx->input_handler->GetKey(KeyCode::KK_S)) direction_.y += 1.0f;
  if (ctx->input_handler->GetKey(KeyCode::KK_A)) direction_.x -= 1.0f;
  if (ctx->input_handler->GetKey(KeyCode::KK_D)) direction_.x += 1.0f;

  // TODO Update Animation
  UpdateAnimation(delta_time);
}

void Player::OnFixedUpdate(GameContext* ctx, float delta_time) {
  // apply input to velocity
  if (direction_.x == 0 && direction_.y == 0) {
    velocity_ = {0, 0};
  }
  else {
    float len = std::sqrt(direction_.x * direction_.x + direction_.y * direction_.y);
    velocity_ = {
      (direction_.x / len) * move_speed_,
      (direction_.y / len) * move_speed_
    };
  }

  // movement
  transform_.position.x += velocity_.x * delta_time;
  transform_.position.y += velocity_.y * delta_time;

  // TODO: Collision
}

void Player::OnRender(GameContext* ctx) {
  auto rr = ctx->render_resource_manager->renderer.get();

  rr->DrawSprite(texture_id_,
                 transform_,
                 uv_,
                 color_);
}

void Player::UpdateAnimation(float delta_time) {
  if (!animation_state_.is_playing || animation_state_.frames.empty()) return;

  animation_state_.current_frame_time += delta_time;
  if (animation_state_.current_frame_time < animation_state_.frame_durations[animation_state_.current_frame]) return;

  animation_state_.current_frame_time -= animation_state_.frame_durations[animation_state_.current_frame];
  animation_state_.current_frame++;

  if (animation_state_.current_frame >= animation_state_.frames.size()) {
    if (animation_state_.is_loop) {
      animation_state_.current_frame = 0;
    }
    else {
      animation_state_.current_frame = animation_state_.frames.size() - 1;
      animation_state_.is_playing = false;
    }
  }

  uv_.position.x = animation_state_.frames[animation_state_.current_frame].u;
  uv_.position.y = animation_state_.frames[animation_state_.current_frame].v;
}
