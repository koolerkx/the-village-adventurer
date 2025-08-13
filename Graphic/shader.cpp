module;
#include <cassert>
#include <d3d11.h>
#include <WinString.h>
#include <DirectXMath.h>

module graphic.shader;

import std;
import graphic.utils.config;

ShaderManager::ShaderManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Dx11WrapperConfig config) {
  if (!pDevice || !pContext) {
    OutputDebugString("ShaderManager::ShaderManager : 与えられたデバイスかコンテキストが不正です");
    assert(false);
  }

  device_ = pDevice;
  device_context_ = pContext;

  // 頂点レイアウトの定義
  D3D11_INPUT_ELEMENT_DESC instance_layout[] = {
    // slot 0: per-vertex
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},

    // slot 1: per-instance
    {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1}, // pos
    {"TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    // size
    {"TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    // uvRect
    {"TEXCOORD", 4, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    // rotation radian
    {"TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    // rotation pivot
    {"TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    // color
  };
  CreateVertexShader(config.instanced_vertex_shader, instance_layout,
                     instance_vertex_shader_.GetAddressOf(), instance_input_layout_.ReleaseAndGetAddressOf());

  // 頂点レイアウトの定義
  D3D11_INPUT_ELEMENT_DESC layout[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };
  CreateVertexShader(config.vertex_shader, layout,
                     vertex_shader_.GetAddressOf(), input_layout_.ReleaseAndGetAddressOf());
  CreateConstantBuffer();

  CreatePixelShader(config.pixel_shader, pixel_shader_.GetAddressOf());
  CreateSamplerState();
}

void ShaderManager::CreateVertexShader(const std::string& filename, std::span<D3D11_INPUT_ELEMENT_DESC> layout,
                                       ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppInputLayout) {
  // 事前コンパイル済み頂点シェーダーの読み込み
  std::ifstream ifs_vs(filename, std::ios::binary);

  if (!ifs_vs) {
    MessageBox(nullptr, ("頂点シェーダーの読み込みに失敗しました\n\n" + filename).c_str(), "エラー", MB_OK);
    assert(false);
  }

  // ファイルサイズを取得
  ifs_vs.seekg(0, std::ios::end);            // ファイルポインタを末尾に移動
  std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
  ifs_vs.seekg(0, std::ios::beg);            // ファイルポインタを先頭に戻す

  // バイナリデータを格納するためのバッファを確保
  unsigned char* vsbinary_pointer = new unsigned char[filesize];

  ifs_vs.read(reinterpret_cast<char*>(vsbinary_pointer), filesize); // バイナリデータを読み込む
  ifs_vs.close();                                                   // ファイルを閉じる

  // 頂点シェーダーの作成
  HRESULT hr = device_->CreateVertexShader(vsbinary_pointer, filesize, nullptr, ppVertexShader);

  if (FAILED(hr)) {
    OutputDebugString("ShaderManager::CreateVertexShader() : 頂点シェーダーの作成に失敗しました");
    delete[] vsbinary_pointer; // メモリリークしないようにバイナリデータのバッファを解放
    assert(false);
  }

  UINT num_elements = static_cast<UINT>(layout.size()); // 配列の要素数を取得

  // 頂点レイアウトの作成
  hr = device_->CreateInputLayout(layout.data(), num_elements, vsbinary_pointer, filesize, ppInputLayout);

  delete[] vsbinary_pointer; // バイナリデータのバッファを解放

  if (FAILED(hr)) {
    OutputDebugString("ShaderManager::CreateVertexShader() : 頂点レイアウトの作成に失敗しました");
    assert(false);
  }
}

void ShaderManager::CreateConstantBuffer() {
  // 頂点シェーダー用定数バッファの作成
  D3D11_BUFFER_DESC buffer_desc{};
  buffer_desc.ByteWidth = sizeof(DirectX::XMFLOAT4X4); // バッファのサイズ
  buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;  // バインドフラグ

  device_->CreateBuffer(&buffer_desc, nullptr, vs_constant_buffer_0_.GetAddressOf());
  device_->CreateBuffer(&buffer_desc, nullptr, vs_constant_buffer_1_.GetAddressOf());
}

void ShaderManager::CreatePixelShader(std::string filename, ID3D11PixelShader** pp_pixel_shader) {
  // 事前コンパイル済みピクセルシェーダーの読み込み
  std::ifstream ifs_ps(filename, std::ios::binary);
  if (!ifs_ps) {
    MessageBox(nullptr, ("ピクセルシェーダーの読み込みに失敗しました\n\n" + filename).c_str(), "エラー", MB_OK);
    assert(false);
  }

  ifs_ps.seekg(0, std::ios::end);
  std::streamsize filesize = ifs_ps.tellg();
  ifs_ps.seekg(0, std::ios::beg);

  unsigned char* psbinary_pointer = new unsigned char[filesize];
  ifs_ps.read(reinterpret_cast<char*>(psbinary_pointer), filesize);
  ifs_ps.close();

  // ピクセルシェーダーの作成
  HRESULT hr = device_->CreatePixelShader(psbinary_pointer, filesize, nullptr, pp_pixel_shader);

  delete[] psbinary_pointer; // バイナリデータのバッファを解放

  if (FAILED(hr)) {
    OutputDebugString("ShaderManager::CreatePixelShader() : ピクセルシェーダーの作成に失敗しました");
    assert(false);
  }
}

void ShaderManager::CreateSamplerState() {
  // サンプラーステート設定
  D3D11_SAMPLER_DESC sampler_desc{};

  // フィルタリング
  sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

  // UV参照外の取り扱い（UVアドレッシングモード）
  sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  sampler_desc.MipLODBias = 0;
  sampler_desc.MaxAnisotropy = 8;
  sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  sampler_desc.MinLOD = 0;
  sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

  device_->CreateSamplerState(&sampler_desc, sampler_state_.GetAddressOf());
}

void ShaderManager::Begin(VertexShaderType type) {
  switch (type) {
  case VertexShaderType::Instance:
    // 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
    device_context_->VSSetShader(instance_vertex_shader_.Get(), nullptr, 0);
    // 頂点レイアウトを描画パイプラインに設定
    device_context_->IASetInputLayout(instance_input_layout_.Get());
    break;
  case VertexShaderType::Default:
    // 頂点シェーダーとピクセルシェーダーを描画パイプラインに設定
    device_context_->VSSetShader(vertex_shader_.Get(), nullptr, 0);
    // 頂点レイアウトを描画パイプラインに設定
    device_context_->IASetInputLayout(input_layout_.Get());
    break;
  }

  device_context_->PSSetShader(pixel_shader_.Get(), nullptr, 0);

  // 定数バッファを描画パイプラインに設定
  device_context_->VSSetConstantBuffers(0, 1, vs_constant_buffer_0_.GetAddressOf());
  device_context_->VSSetConstantBuffers(1, 1, vs_constant_buffer_1_.GetAddressOf());

  // サンプラーステートを描画パイプラインに設定
  device_context_->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());
}

void ShaderManager::SetProjectionMatrix(const DirectX::XMMATRIX& matrix) const {
  // 定数バッファ格納用行列の構造体を定義
  DirectX::XMFLOAT4X4 transpose;

  // 行列を転置して定数バッファ格納用行列に変換
  XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

  // 定数バッファに行列をセット
  device_context_->UpdateSubresource(vs_constant_buffer_0_.Get(), 0, nullptr, &transpose, 0, 0);
}

void ShaderManager::SetWorldMatrix(const DirectX::XMMATRIX& matrix) const {
  DirectX::XMFLOAT4X4 transpose;

  XMStoreFloat4x4(&transpose, XMMatrixTranspose(matrix));

  device_context_->UpdateSubresource(vs_constant_buffer_1_.Get(), 0, nullptr, &transpose, 0, 0);
}
