module;
#include "stdint.h"

export module game.scene.splash_scene;

import std;
import game.scene;
import graphic.utils.font;

enum class State: uint8_t {
  AUTHOR_PRESENT,
  RECOMMENDATION
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
  
  const float uv_offset_speed_ = 25.0f;
  float uv_horizontal_offset_ = 0;

public:
  SplashScene();
  
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
