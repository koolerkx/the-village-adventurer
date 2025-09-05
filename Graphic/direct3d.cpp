module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cassert>

#include <d3d11.h>
#include <dxgi1_6.h>

#include <wrl.h>

module graphic.direct3D;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

import std;
import graphic.utils.types;

HRESULT Dx11Wrapper::CreateSwapChain(HWND hwnd) {
  DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
  swapchain_desc.BufferCount = 2; // 裏画面が何個用意する
  swapchain_desc.Width = win_size_.cx;
  swapchain_desc.Height = win_size_.cy;
  swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // 色のformat
  swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 何に使う、ここは絵を各場所で使う
  swapchain_desc.SampleDesc.Count = 1;
  swapchain_desc.SampleDesc.Quality = 0;
  swapchain_desc.SwapEffect = config_.vsync ? DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
  // swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

  HRESULT hr = dxgi_factory_->CreateSwapChainForHwnd(
    device_.Get(),
    hwnd,
    &swapchain_desc,
    nullptr,
    nullptr,
    swapchain_.ReleaseAndGetAddressOf()
  );

  return hr;
}

HRESULT Dx11Wrapper::InitializeDXGIDevice() {
  HRESULT hr = S_FALSE;

  UINT device_flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
  device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  hr = CreateDXGIFactory2(device_flags, IID_PPV_ARGS(dxgi_factory_.ReleaseAndGetAddressOf()));
  if (FAILED(hr)) {
    OutputDebugString("Failed to create DXGIFactory\n");
    return hr;
  }

  D3D_FEATURE_LEVEL levels[] = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0
  };

  D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

  hr = D3D11CreateDevice(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    device_flags,
    levels,
    ARRAYSIZE(levels),
    D3D11_SDK_VERSION,
    device_.GetAddressOf(),
    &feature_level,
    device_context_.GetAddressOf()
  );

  return hr;
}

HRESULT Dx11Wrapper::CreateFinalRenderTargets() {
  HRESULT hr = S_FALSE;

  ComPtr<ID3D11Texture2D> back_buffer_pointer = nullptr;

  // バックバッファの取得
  hr = swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D),
                             reinterpret_cast<void**>(back_buffer_pointer.GetAddressOf()));
  if (FAILED(hr)) return hr;

  // バックバッファのレンダーターゲットビューの生成
  hr = device_->CreateRenderTargetView(back_buffer_pointer.Get(), nullptr, render_target_views_.GetAddressOf());
  back_buffer_pointer->GetDesc(&back_buffer_desc_);

  return hr;
}

HRESULT Dx11Wrapper::CreateDepthStencilView() {
  HRESULT hr = S_FALSE;

  D3D11_TEXTURE2D_DESC depth_stencil_desc;
  depth_stencil_desc.Width = win_size_.cx;
  depth_stencil_desc.Height = win_size_.cy;
  depth_stencil_desc.MipLevels = 1;
  depth_stencil_desc.ArraySize = 1;
  depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depth_stencil_desc.SampleDesc.Count = 1;
  depth_stencil_desc.SampleDesc.Quality = 0;
  depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
  depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depth_stencil_desc.CPUAccessFlags = 0;
  depth_stencil_desc.MiscFlags = 0;
  hr = device_->CreateTexture2D(&depth_stencil_desc, nullptr, depth_stencil_buffer_.ReleaseAndGetAddressOf());
  if (FAILED(hr)) return hr;

  D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
  depth_stencil_view_desc.Format = depth_stencil_desc.Format;
  depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depth_stencil_view_desc.Texture2D.MipSlice = 0;
  depth_stencil_view_desc.Flags = 0;
  hr = device_->CreateDepthStencilView(depth_stencil_buffer_.Get(), &depth_stencil_view_desc,
                                       depth_stencil_view_.GetAddressOf());

  if (FAILED(hr)) return hr;
  return hr;
}

void Dx11Wrapper::CreateDepthStencilState() {
  SetEnableDepth(false);
}

void Dx11Wrapper::CreateViewport() {
  viewport_.TopLeftX = 0.0f;
  viewport_.TopLeftY = 0.0f;
  viewport_.Width = static_cast<FLOAT>(win_size_.cx);
  viewport_.Height = static_cast<FLOAT>(win_size_.cy);
  viewport_.MinDepth = 0.0f;
  viewport_.MaxDepth = 1.0f;
  device_context_->RSSetViewports(1, &viewport_); // ビューポートの設定
}

void Dx11Wrapper::CreateRasterizerState() {
  D3D11_RASTERIZER_DESC raster_desc;

  raster_desc.FillMode = D3D11_FILL_SOLID;
  raster_desc.CullMode = D3D11_CULL_BACK;
  raster_desc.FrontCounterClockwise = false;
  raster_desc.DepthClipEnable = TRUE;
  raster_desc.ScissorEnable = true;
  raster_desc.MultisampleEnable = false;
  raster_desc.AntialiasedLineEnable = false;

  raster_desc.DepthBias = 0;
  raster_desc.DepthBiasClamp = 0.0f;
  raster_desc.SlopeScaledDepthBias = 0.0f;

  device_->CreateRasterizerState(&raster_desc, rasterizer_state_.GetAddressOf());
  device_context_->RSSetState(rasterizer_state_.Get());

  D3D11_RECT scissor_rect = {
    .left = 0,
    .top = 0,
    .right = win_size_.cx,
    .bottom = win_size_.cy
  };

  device_context_->RSSetScissorRects(1, &scissor_rect);
}

Dx11Wrapper::Dx11Wrapper(HWND hwnd, const Dx11WrapperConfig& config) {
  config_ = config;
  win_size_.cx = config.window_size_width;
  win_size_.cy = config.window_size_height;

  if (FAILED(InitializeDXGIDevice())) {
    assert(0);
    return;
  }
  if (FAILED(CreateSwapChain(hwnd))) {
    assert(0);
    return;
  }
  if (FAILED(CreateFinalRenderTargets())) {
    assert(0);
    return;
  }
  if (FAILED(CreateDepthStencilView())) {
    assert(0);
    return;
  }
  CreateDepthStencilState();
  CreateViewport();
  CreateRasterizerState();

  std::unique_ptr<ShaderManager> shader_manager = std::make_unique<ShaderManager>(
    device_.Get(), device_context_.Get(), config_);
  std::unique_ptr<TextureManager> texture_manager = std::make_unique<TextureManager>(
    device_.Get(), device_context_.Get());
  std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>(device_.Get(), device_context_.Get(),
                                                                  shader_manager.get(), texture_manager.get(),
                                                                  win_size_, 12);

  resource_manager_ = std::make_unique<ResourceManager>();
  resource_manager_->shader_manager = std::move(shader_manager);
  resource_manager_->texture_manager = std::move(texture_manager);
  resource_manager_->renderer = std::move(renderer);
}

Dx11Wrapper::~Dx11Wrapper() {}

void Dx11Wrapper::BeginDraw() {
  float clear_color[4] = {
    background_clear_color.x, background_clear_color.y, background_clear_color.z, background_clear_color.w
  };
  device_context_->ClearRenderTargetView(render_target_views_.Get(), clear_color);
  device_context_->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

  // レンダーターゲットビューとデプスステンシルビューの設定 
  device_context_->OMSetRenderTargets(1,
                                      render_target_views_.GetAddressOf(),
                                      depth_stencil_view_.Get());
}

void Dx11Wrapper::EndDraw() const {
  (void)swapchain_->Present(config_.vsync ? 1 : 0, 0);
}

void Dx11Wrapper::Dispatch(std::move_only_function<void(ResourceManager*)> func) {
  func(resource_manager_.get());
}

ComPtr<ID3D11Device> Dx11Wrapper::GetDevice() {
  return device_;
}

ComPtr<ID3D11DeviceContext> Dx11Wrapper::GetDeviceContext() {
  return device_context_;
}

ComPtr<IDXGISwapChain> Dx11Wrapper::GetSwapchain() {
  return swapchain_;
}

void Dx11Wrapper::SetBlendMultiply(ID3D11DeviceContext* ctx, ID3D11BlendState* state,
                                   float r, float g, float b, float a) {
  float factor[4] = {r, g, b, a};
  ctx->OMSetBlendState(state, factor, 0xffffffff);
}

ResourceManager* Dx11Wrapper::GetResourceManager() const {
  return resource_manager_.get();
}

void Dx11Wrapper::SetEnableDepth(bool enable) {
  D3D11_DEPTH_STENCIL_DESC dsd = {};
  dsd.DepthFunc = D3D11_COMPARISON_LESS;
  dsd.StencilEnable = FALSE;

  if (enable) {
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  }
  else {
    dsd.DepthEnable = FALSE; // 無効にする
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  }
  device_->CreateDepthStencilState(&dsd, depth_stencil_state_depth_disable_.GetAddressOf());
  device_context_->OMSetDepthStencilState(depth_stencil_state_depth_disable_.Get(), NULL);
}

void Dx11Wrapper::SetFullscreen(bool enable) {
  swapchain_->SetFullscreenState(enable, NULL);
}
