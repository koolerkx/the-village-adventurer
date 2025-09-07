module;
#include "stdint.h"

export module game.scene.splash_scene;

import std;
import game.scene;
import graphic.utils.font;

enum class State: uint8_t {
  RECOMMENDATION,
  SWIRL,
  AUTHOR_PRESENT,
};

export class SplashScene : public IScene {
private:
  std::wstring font_key_;
  Font* default_font_;

  FixedPoolIndexType background_1_texture_id_;
  FixedPoolIndexType background_2_texture_id_;
  FixedPoolIndexType background_3_texture_id_;
  FixedPoolIndexType background_4_texture_id_;
  
  FixedPoolIndexType ui_texture_id_;
  
  float uv_horizontal_offset_ = 0;

  float swirl_twists_ = 1;
  float swirl_radius_ = 0.25;
  const float swirl_speed_ = 0.3;
  
  State state_ = State::RECOMMENDATION;

  float recommendation_opacity_ = 0.0f;
  float recommendation_timeout_ = 1.0f;
  float overlay_opacity_ = 1.0f;

  float name_opacity_ = 0.0f;
  float name_timeout_ = 1.0f;
  
public:
  SplashScene();
  
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
