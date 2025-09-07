module;
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

export module graphic.shader;

import std;
import graphic.utils.config;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

export enum class VertexShaderType {
  Instance,
  Default
};

export enum class PixelShaderType {
  Default,
  Swirl
};

struct alignas(32) SwirlParams {
  DirectX::XMFLOAT2 swirlCenter1; // (u, v)
  float swirlRadius1;
  float swirlTwists1;
  DirectX::XMFLOAT2 swirlCenter2; // (u, v)
  float swirlRadius2;
  float swirlTwists2;
};

export class ShaderManager {
private:
  ComPtr<ID3D11Device> device_ = nullptr;
  ComPtr<ID3D11DeviceContext> device_context_ = nullptr;

  // Vertex Shader
  ComPtr<ID3D11VertexShader> vertex_shader_ = nullptr;
  ComPtr<ID3D11InputLayout> input_layout_ = nullptr;
  ComPtr<ID3D11Buffer> vs_constant_buffer_0_ = nullptr; // Projection
  ComPtr<ID3D11Buffer> vs_constant_buffer_1_ = nullptr; // World

  // Pixel Shader
  ComPtr<ID3D11Buffer> ps_cbuffer_swirl_;

  // instace draw shder
  ComPtr<ID3D11VertexShader> instance_vertex_shader_ = nullptr;
  ComPtr<ID3D11InputLayout> instance_input_layout_ = nullptr;

  ComPtr<ID3D11SamplerState> sampler_state_ = nullptr;

  void CreateVertexShader(const std::string& filename,
                          std::span<D3D11_INPUT_ELEMENT_DESC> layout,
                          ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppInputLayout);
  void CreateConstantBuffer();

  // Pixel Shader
  ComPtr<ID3D11PixelShader> pixel_shader_ = nullptr;
  ComPtr<ID3D11PixelShader> swirl_pixel_shader_ = nullptr;

  void CreatePixelShader(std::string filename, ID3D11PixelShader** pp_pixel_shader);
  void CreatePSConstantBuffers();

  void CreateSamplerState();

public:
  ShaderManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Dx11WrapperConfig config);

  void Begin(VertexShaderType type = VertexShaderType::Default, PixelShaderType ps_type = PixelShaderType::Default);

  void SetProjectionMatrix(const DirectX::XMMATRIX& matrix) const;
  void SetWorldMatrix(const DirectX::XMMATRIX& matrix) const;
  void SetSwirlShader(const DirectX::XMFLOAT2 center1, const float radius1, const float twists1,
                      const DirectX::XMFLOAT2 center2, const float radius2, const float twists2) const;
};
