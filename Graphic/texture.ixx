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
  ComPtr<ID3D11ShaderResourceView> texture;
  TextureSize size;
};

export class TextureManager {
private:
  ComPtr<ID3D11Device> device_ = nullptr;
  ComPtr<ID3D11DeviceContext> device_context_ = nullptr;

  FixedPool<Texture> texture_pool_;

public:
  TextureManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

  FixedPoolIndexType Load(std::wstring filename);
  void Release(FixedPoolIndexType idx);

  void Set(FixedPoolIndexType idx);

  TextureSize GetSize(FixedPoolIndexType idx);
};
