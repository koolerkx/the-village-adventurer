module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_6.h>

#include <wrl.h>

// TODO: Write to vcxproj
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

export module graphic.direct3D;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

import std;
export import graphic.render;
export import graphic.shader;
export import graphic.texture;

export struct ResourceManager {
  std::unique_ptr<Renderer> renderer = nullptr;
  std::unique_ptr<ShaderManager> shader_manager = nullptr;
  std::unique_ptr<TextureManager> texture_manager = nullptr;
};

export class Dx11Wrapper {
  SIZE win_size_;

  // DXGI
  ComPtr<IDXGIFactory7> dxgi_factory_ = nullptr;
  ComPtr<IDXGISwapChain1> swapchain_ = nullptr;

  // Direct3D 11
  ComPtr<ID3D11Device> device_ = nullptr;
  ComPtr<ID3D11DeviceContext> device_context_ = nullptr;

  ComPtr<ID3D11BlendState> blend_state_multiply_ = nullptr;
  ComPtr<ID3D11DepthStencilState> depth_stencil_state_depth_disable_ = nullptr;

  // Render Targets & Depth
  ComPtr<ID3D11RenderTargetView> render_target_views_ = nullptr;
  ComPtr<ID3D11Texture2D> depth_stencil_buffer_ = nullptr;
  ComPtr<ID3D11DepthStencilView> depth_stencil_view_ = nullptr;
  D3D11_TEXTURE2D_DESC back_buffer_desc_{};

  D3D11_VIEWPORT viewport_{};

  HRESULT CreateSwapChain(HWND hwnd);
  HRESULT InitializeDXGIDevice();
  HRESULT CreateFinalRenderTargets();
  HRESULT CreateDepthStencilView();
  void CreateBlendState();
  void CreateDepthStencilState();
  void CreateViewport();

  std::unique_ptr<ResourceManager> resource_manager_ = nullptr;

public:
  Dx11Wrapper(HWND hwnd);
  ~Dx11Wrapper();

  void BeginDraw();
  void EndDraw() const;

  void Dispatch(
    std::move_only_function<void(ResourceManager*)> func = [](ResourceManager*) {}
  );

  // Getters
  ComPtr<ID3D11Device> GetDevice();
  ComPtr<ID3D11DeviceContext> GetDeviceContext();
  ComPtr<IDXGISwapChain> GetSwapchain();

  static void SetBlendMultiply(ID3D11DeviceContext* ctx, ID3D11BlendState* state,
                               float r = 0, float g = 0, float b = 0, float a = 0);

  SIZE GetWindowSize() const { return win_size_; }
  ResourceManager* GetResourceManager() const;
};
