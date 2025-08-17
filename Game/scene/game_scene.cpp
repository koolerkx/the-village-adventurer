module;

module game.scene_game;

import std;
import game.scene_manager;
import graphic.utils.types;
import game.map.tile_repository;
import game.scene_object;

void GameScene::OnEnter(GameContext* ctx) {
  std::cout << "GameScene> OnEnter" << std::endl;

  std::string default_map = SceneManager::GetInstance().GetGameConfig()->default_map;
  TileRepository* tr = SceneManager::GetInstance().GetTileRepository();

  map_.reset(new TileMap());
  std::string default_map_path = "map/map_data/" + default_map + ".tmx";

  std::string texture_path = SceneManager::GetInstance().GetGameConfig()->map_texture_filepath;
  std::wstring w_texture_path = std::wstring(texture_path.begin(), texture_path.end());

  FixedPoolIndexType id = ctx->render_resource_manager->texture_manager->Load(w_texture_path);
  map_->Load(default_map_path, id, tr);
  Transform t = map_->GetTransform();

  // TODO: remove debug data
  t.position.x = 0.0f;
  t.position.y = -384.0f;
  t.scale.x = 3.0f;
  t.scale.y = 3.0f;
  t.position_anchor.x = 0.0f;
  t.position_anchor.y = 0.0f;
  map_->SetTransform(t);

  // Player Initialize
  player_ = std::make_unique<Player>();
  scene_object::LoadTexture(
    player_->texture_id,
    player_->texture_path,
    ctx->render_resource_manager->texture_manager.get());
}

void GameScene::OnUpdate(GameContext* ctx, float delta_time) {
  // std::cout << "GameScene> OnUpdate: " << delta_time << std::endl;
  player_->HandleMovement(ctx->input_handler, delta_time);

  map_->OnUpdate(ctx, delta_time);
  scene_object::UpdateAnimation(player_->animation_state, delta_time, player_->uv);
}

void GameScene::OnRender(GameContext* ctx) {
  // std::cout << "GameScene> OnRender" << std::endl;

  map_->OnRender(ctx);
  scene_object::Render(RenderItem{
                         .texture_id = player_->texture_id,
                         .transform = player_->transform,
                         .uv = player_->uv,
                         .color = player_->color
                       }, ctx->render_resource_manager->renderer.get());
}

void GameScene::OnExit(GameContext*) {
  std::cout << "GameScene> OnExit" << std::endl;
}
