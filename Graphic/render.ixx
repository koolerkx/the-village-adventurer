module;
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

export module graphic.render;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

import std;
import graphic.shader;
import graphic.utils.math;

// 頂点構造体
export struct Vertex {
  POSITION position; // 頂点座標
  COLOR color;       // 色
  UV uv;             // テクスチャー
};

export struct Transform {
  POSITION position;
  SCALE scale = {1.0f, 1.0f};
  SCALE size;
  float rotation_radian = 0.0f;
  POSITION rotation_pivot = {0.0f, 0.0f, 0.0f};
};

export class Renderer {
private:
  ComPtr<ID3D11Device> device_ = nullptr;
  ComPtr<ID3D11DeviceContext> device_context_ = nullptr;
  std::shared_ptr<ShaderManager> shader_manager_ = nullptr;

  ComPtr<ID3D11Buffer> vertex_buffer_ = nullptr; // 頂点バッファ

  int vertex_num_ = 0;

  static DirectX::XMMATRIX MakeTransformMatrix(const Transform& transform);

public:
  Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
           const std::shared_ptr<ShaderManager>& shader_manager, int vertex_num = 4);

  void Draw(const Transform& transform, const COLOR& color);
};
