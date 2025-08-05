module;
#include <cassert>
#include <WinString.h>

module graphic.texture;

TextureManager::TextureManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
  if (!pDevice || !pContext) {
    OutputDebugString("ShaderManager::ShaderManager : 与えられたデバイスかコンテキストが不正です");
    assert(false);
  }

  device_ = pDevice;
  device_context_ = pContext;
}

int TextureManager::Load(const std::string& filename) {}

void TextureManager::Release(FixedPoolIndexType idx) {
  texture_pool_.remove(idx, [](Texture texture) -> void { texture.texture.Reset(); });
}

void TextureManager::Set(FixedPoolIndexType idx) {
  Texture* texture = texture_pool_.get(idx);
  device_context_->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
}

TextureSize TextureManager::GetSize(FixedPoolIndexType idx) {
  Texture* texture = texture_pool_.get(idx);
  return {
    .height = texture->height,
    .width = texture->width
  };
}
