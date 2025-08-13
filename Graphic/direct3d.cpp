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
  swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 0: 垂直同期なし
  // swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
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

void Dx11Wrapper::CreateBlendState() {
  // RGB A -> 好きに使っていい値、基本は透明の表現に使う
  // αテスト、αブレンド
  // ブレンドステート設定
  D3D11_BLEND_DESC bd = {};
  bd.AlphaToCoverageEnable = FALSE;
  bd.IndependentBlendEnable = FALSE;
  bd.RenderTarget[0].BlendEnable = TRUE; // αブレンドするしない

  // src ... ソース（今から描く絵（色）） dest　...　すでに絵描かれた絵（色）

  // RGB
  bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // 演算子
  // SrcRGB * SrcBlend + DestRGB * (1 - DestBlend)

  // A
  bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  // SrcBlendAlpha * 1 + DestBlendAlpha * 0

  bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  device_->CreateBlendState(&bd, blend_state_multiply_.GetAddressOf());

  SetBlendMultiply(device_context_.Get(), blend_state_multiply_.Get());
}

void Dx11Wrapper::CreateDepthStencilState() {
  // 深度ステンシルステート設定
  D3D11_DEPTH_STENCIL_DESC dsd = {};
  dsd.DepthFunc = D3D11_COMPARISON_LESS;
  dsd.StencilEnable = FALSE;
  dsd.DepthEnable = FALSE; // 無効にする
  dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

  device_->CreateDepthStencilState(&dsd, depth_stencil_state_depth_disable_.GetAddressOf());

  // dsd.DepthEnable = TRUE;
  // dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  // g_pDevice->CreateDepthStencilState(&dsd, &g_pDepthStencilStateDepthEnable);

  device_context_->OMSetDepthStencilState(depth_stencil_state_depth_disable_.Get(), NULL);
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
  CreateBlendState();
  CreateDepthStencilState();
  CreateViewport();

  std::unique_ptr<ShaderManager> shader_manager = std::make_unique<ShaderManager>(device_.Get(), device_context_.Get(), config_);
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
  float clear_color[4] = {background_clear_color.x, background_clear_color.y, background_clear_color.z, background_clear_color.w};
  device_context_->ClearRenderTargetView(render_target_views_.Get(), clear_color);
  device_context_->ClearDepthStencilView(depth_stencil_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

  // レンダーターゲットビューとデプスステンシルビューの設定 
  device_context_->OMSetRenderTargets(1,
                                      render_target_views_.GetAddressOf(),
                                      depth_stencil_view_.Get());
}

void Dx11Wrapper::EndDraw() const {
  // (void)swapchain_->Present(1, 0);
  (void)swapchain_->Present(0, 0);
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
