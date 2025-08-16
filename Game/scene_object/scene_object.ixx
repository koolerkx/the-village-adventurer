module;

export module game.scene_object;

import std;
import graphic.direct3D;
import game.types;
import graphic.utils.types;

export namespace scene_object {
  void LoadTexture(FixedPoolIndexType& id, const std::wstring_view texture_path, TextureManager* tm) {
    id = tm->Load(texture_path.data());
  }

  void Render(const RenderItem& render_item, Renderer* renderer) {
    renderer->DrawSprite(render_item.texture_id, render_item.transform, render_item.uv, render_item.color);
  }
}
