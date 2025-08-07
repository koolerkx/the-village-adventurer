module;
#include <d3d11.h>
#include <wrl/client.h>

export module graphic.texture;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

import std;
export import graphic.utils.fixed_pool;
import graphic.utils.math;

export struct TextureSize {
  unsigned int width;
  unsigned int height;
};

/**
 * @struct Texture
 * @brief Texture Ç™ä«óùÇ∑ÇÈÇ‡ÇÃ
 */
export struct Texture {
  std::wstring filename;
  std::optional<std::string> key;
  ComPtr<ID3D11ShaderResourceView> texture;
  TextureSize size;
};

export class TextureManager {
private:
  ComPtr<ID3D11Device> device_ = nullptr;
  ComPtr<ID3D11DeviceContext> device_context_ = nullptr;

  FixedPool<Texture> texture_pool_;
  std::unordered_map<std::wstring, FixedPoolIndexType> texture_filename_;
  std::unordered_map<std::string, FixedPoolIndexType> texture_key_;

public:
  TextureManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

  FixedPoolIndexType Load(std::wstring filename, std::optional<std::string> key = std::nullopt);
  void Release(FixedPoolIndexType idx);

  void SetShaderById(FixedPoolIndexType idx);
  void SetShaderByKey(std::string key);
  inline void SetShader(FixedPoolIndexType idx) { SetShaderById(idx); }

  TextureSize GetSizeById(FixedPoolIndexType idx);
  TextureSize GetSizeByKey(std::string key);
  inline void GetSize(FixedPoolIndexType idx) { GetSizeById(idx); }

  FixedPoolIndexType GetIdByKey(std::string key);
};
