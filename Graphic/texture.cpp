module;
#include <cassert>
#include <WinString.h>
#include "utils/WICTextureLoader11.h"

module graphic.texture;

TextureManager::TextureManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
  if (!pDevice || !pContext) {
    OutputDebugString("ShaderManager::ShaderManager : 与えられたデバイスかコンテキストが不正です");
    assert(false);
  }

  device_ = pDevice;
  device_context_ = pContext;
}

FixedPoolIndexType TextureManager::Load(std::wstring filename, std::optional<std::string> key) {
  if (texture_filename_.contains(filename)) {
    return texture_filename_[filename];
  }
  if (key.has_value() && texture_key_.contains(key.value())) {
    return texture_key_[key.value()];
  }

  ComPtr<ID3D11Resource> texture = nullptr;
  ComPtr<ID3D11ShaderResourceView> texture_resource_view = nullptr;

  if (FAILED(
    DirectX::CreateWICTextureFromFile(device_.Get(), filename.c_str(), texture.GetAddressOf(), texture_resource_view.
      GetAddressOf()))) {
    MessageBoxW(nullptr, L"フォントテクスチャの読み込みに失敗しました", filename.c_str(), MB_OK | MB_ICONERROR);
    assert(false);
  }

  D3D11_TEXTURE2D_DESC texture2d_desc;
  static_cast<ID3D11Texture2D*>(texture.Get())->GetDesc(&texture2d_desc);
  const UINT texture_width = texture2d_desc.Width;
  const UINT texture_height = texture2d_desc.Height;

  auto result = texture_pool_.insert({
    .filename = filename,
    .key = key,
    .texture = texture_resource_view,
    .size{
      .width = texture_width,
      .height = texture_height
    }
  });

  if (!result.has_value()) {
    MessageBoxW(nullptr, L"テクスチャの挿入に失敗しました", filename.c_str(), MB_OK | MB_ICONERROR);
    assert(false);
  }

  texture_filename_[filename] = result.value();
  if (key.has_value()) {
    texture_key_[key.value()] = result.value();
  }

  return result.value();
}

void TextureManager::Release(FixedPoolIndexType idx) {
  Texture* texture = texture_pool_.get(idx);
  texture_filename_.erase(texture->filename);
  texture_key_.erase(texture->key.value_or(""));
  texture_pool_.remove(idx, [](Texture texture) -> void { texture.texture.Reset(); });
}

void TextureManager::SetShaderById(FixedPoolIndexType idx) {
  Texture* texture = texture_pool_.get(idx);
  device_context_->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
}

void TextureManager::SetShaderByKey(std::string key) {
  FixedPoolIndexType idx = texture_key_[key];
  Texture* texture = texture_pool_.get(idx);
  device_context_->PSSetShaderResources(0, 1, texture->texture.GetAddressOf());
}

TextureSize TextureManager::GetSizeById(FixedPoolIndexType idx) {
  Texture* texture = texture_pool_.get(idx);
  return texture->size;
}

TextureSize TextureManager::GetSizeByKey(std::string key) {
  FixedPoolIndexType idx = texture_key_[key];
  Texture* texture = texture_pool_.get(idx);
  return texture->size;
}

FixedPoolIndexType TextureManager::GetIdByKey(std::string key) {
  return texture_key_[key];
}
