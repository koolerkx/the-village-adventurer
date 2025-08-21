module;

module game.scene_object.player;

import std;
import game.scene_object.camera;
import game.scene_game.context;
import game.collision_handler;
import game.map.field_object;
import game.scene_object.skill;
import game.utils.throttle;

// Texture data
static constexpr std::wstring_view texture_path = L"assets/character_01.png"; // TODO: extract
static constexpr PlayerState default_state = PlayerState::IDLE_UP;

Player::Player(GameContext* ctx, SceneContext*) {
  const auto tm = ctx->render_resource_manager->texture_manager.get();

  texture_id_ = tm->Load(texture_path.data());

  SetState(default_state);

  collider_ = {
    .position = {
      transform_.position.x + transform_.position_anchor.x, transform_.position.y + transform_.position_anchor.y
    },
    .rotation = 0,
    .owner = this,
    .shape = RectCollider{
      .x = COLLIDER_PADDING,
      .y = COLLIDER_PADDING,
      .width = COLLISION_DATA.width - COLLIDER_PADDING * 2,
      .height = COLLISION_DATA.height - COLLIDER_PADDING * 2,
    }
  };
}

void Player::OnUpdate(GameContext* ctx, SceneContext* scene_ctx, float delta_time) {
  // Handle Input
  direction_ = {0, 0};
  if (ctx->input_handler->GetKey(KeyCode::KK_W)) direction_.y -= 1.0f;
  if (ctx->input_handler->GetKey(KeyCode::KK_S)) direction_.y += 1.0f;
  if (ctx->input_handler->GetKey(KeyCode::KK_A)) direction_.x -= 1.0f;
  if (ctx->input_handler->GetKey(KeyCode::KK_D)) direction_.x += 1.0f;

  if (direction_.x != 0 || direction_.y != 0) {
    direction_facing_ = direction_;
  }

  const float player_rotation = scene_object::GetPlayerRotationByDirection(direction_facing_); // Right = 0

  if (ctx->input_handler->GetKey(KeyCode::KK_SPACE) && attack_throttle_.CanCall())
    scene_ctx->skill_manager->PlaySkill(
      SKILL_TYPE::NORMAL_ATTACK,
      {transform_.position.x, transform_.position.y},
      player_rotation
    );

  UpdateAnimation(delta_time);
}

void Player::OnFixedUpdate(GameContext*, SceneContext*, float) {
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

  UpdateState();
}


void Player::OnRender(GameContext* ctx, SceneContext*, Camera* camera) {
  auto rr = ctx->render_resource_manager->renderer.get();

  CameraProps props = camera->GetCameraProps();
  // props.algin_pivot = AlginPivot::CENTER_CENTER;
  rr->DrawSprite(RenderItem{
                   texture_id_,
                   transform_,
                   uv_,
                   color_
                 }, props);

  // DEBUG: draw collider
#if defined(DEBUG) || defined(_DEBUG)
  Collider<Player> collider = GetCollider();
  RectCollider shape = std::get<RectCollider>(collider.shape);
  rr->DrawBox(Rect{
                {
                  collider.position.x + shape.x,
                  collider.position.y + shape.y, 0
                },
                {
                  collider.position.x + shape.x + shape.width,
                  collider.position.y + shape.y + shape.height, 0
                },
                color::red
              }, props, true);
#endif
}

void Player::SetState(PlayerState state) {
  if (state_ == state) return;

  state_ = state;

  // set animation by state
  if (auto result = animation_data.find(state); result != animation_data.end()) {
    auto data = result->second;

    animation_state_.is_loop = true;
    animation_state_.is_playing = true;
    animation_state_.frames = data.frames;
    animation_state_.frame_durations = data.frame_durations;
    animation_state_.current_frame = 0;
    animation_state_.current_frame_time = 0 + data.frame_durations[0]; // workaround: force update first frame
  }
}

void Player::UpdateState() {
  if (direction_.x != 0 || direction_.y != 0) {
    if (direction_.y < 0) SetState(PlayerState::MOVE_DOWN);
    else if (direction_.y > 0) SetState(PlayerState::MOVE_UP);
    else if (direction_.x < 0) SetState(PlayerState::MOVE_LEFT);
    else if (direction_.x > 0) SetState(PlayerState::MOVE_RIGHT);
  }
  else {
    if (state_ == PlayerState::MOVE_DOWN) SetState(PlayerState::IDLE_DOWN);
    else if (state_ == PlayerState::MOVE_UP) SetState(PlayerState::IDLE_UP);
    else if (state_ == PlayerState::MOVE_LEFT) SetState(PlayerState::IDLE_LEFT);
    else if (state_ == PlayerState::MOVE_RIGHT) SetState(PlayerState::IDLE_RIGHT);
    else state_ = PlayerState::IDLE_UP;
  }
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

  uv_.position.x = static_cast<float>((animation_state_.frames[animation_state_.current_frame].u));
  uv_.position.y = static_cast<float>(animation_state_.frames[animation_state_.current_frame].v);
}

Vector2 Player::GetVelocity() const {
  return velocity_;
}
