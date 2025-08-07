module;
#include <d3d11.h>
#include <DirectXMath.h>
#include <cassert>
#include <WinString.h>

module graphic.render;

import graphic.texture;

Renderer::Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                   ShaderManager* shader_manager,
                   TextureManager* texture_manager,
                   int vertex_num) {
  if (!pDevice || !pContext) {
    OutputDebugString("ShaderManager::ShaderManager : 与えられたデバイスかコンテキストが不正です");
    assert(false);
  }

  device_ = pDevice;
  device_context_ = pContext;
  shader_manager_ = shader_manager;
  texture_manager_ = texture_manager;

  vertex_num_ = vertex_num;

  // 頂点バッファ生成
  D3D11_BUFFER_DESC bd = {};
  bd.Usage = D3D11_USAGE_DYNAMIC;
  bd.ByteWidth = sizeof(Vertex) * vertex_num_;
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  device_->CreateBuffer(&bd, NULL, vertex_buffer_.GetAddressOf());
}

DirectX::XMMATRIX Renderer::MakeTransformMatrix(const Transform& transform) {
  using namespace DirectX;
  return XMMatrixTransformation2D(
    XMVectorSet(0, 0, 0, 0),                                                                       // 拡大縮小ピボットポイント
    0.0f,                                                                                          // 拡大縮小軸
    XMVectorSet(transform.size.x * transform.scale.x, transform.size.y * transform.scale.y, 0, 0), // 拡大縮小
    XMVectorSet(transform.rotation_pivot.x, transform.rotation_pivot.y, 0, 0),                     // 回転ピボットポイント
    transform.rotation_radian,                                                                     // 回転角度
    XMVectorSet(transform.position.x + transform.size.x * transform.scale.x / 2,
                transform.position.y + transform.scale.y / 2, 0,
                0) // 平行移動
  );
}

void Renderer::Draw(const Transform& transform, const COLOR& color) {
  // シェーダーを描画パイプラインに設定
  shader_manager_->Begin();

  // 頂点バッファをロックする
  D3D11_MAPPED_SUBRESOURCE msr;
  device_context_->Map(vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

  // 頂点バッファへの仮想ポインタを取得
  Vertex* v = static_cast<Vertex*>(msr.pData);

  // 画面の左上から右下に向かう線分を描画する
  v[0].position = {-0.5f, -0.5f, 0.0f}; // LT
  v[1].position = {+0.5f, -0.5f, 0.0f}; // RT
  v[2].position = {-0.5f, +0.5f, 0.0f}; // LB
  v[3].position = {+0.5f, +0.5f, 0.0f}; // RB

  v[0].color = color;
  v[1].color = color;
  v[2].color = color;
  v[3].color = color;

  // 頂点バッファのロックを解除
  device_context_->Unmap(vertex_buffer_.Get(), 0);

  // 頂点バッファを描画パイプラインに設定
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  device_context_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);

  // 頂点シェーダーに変換行列を設定
  shader_manager_->SetProjectionMatrix(
    DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1.0f));

  DirectX::XMMATRIX mat = MakeTransformMatrix(transform);

  shader_manager_->SetWorldMatrix(mat);

  // プリミティブトポロジ設定
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // ポリゴン描画命令発行
  device_context_->Draw(vertex_num_, 0);
}

void Renderer::DrawSprite(const FixedPoolIndexType texture_id,
                          const Transform& transform, const UV& uv,
                          const COLOR& color) {
  texture_manager_->SetShaderById(texture_id);
  shader_manager_->Begin();

  // 頂点バッファをロックする
  D3D11_MAPPED_SUBRESOURCE msr;
  device_context_->Map(vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

  // 頂点バッファへの仮想ポインタを取得
  Vertex* v = static_cast<Vertex*>(msr.pData);

  // 画面の左上から右下に向かう線分を描画する
  v[0].position = {-0.5f, -0.5f, 0.0f}; // LT
  v[1].position = {+0.5f, -0.5f, 0.0f}; // RT
  v[2].position = {-0.5f, +0.5f, 0.0f}; // LB
  v[3].position = {+0.5f, +0.5f, 0.0f}; // RB

  v[0].color = color;
  v[1].color = color;
  v[2].color = color;
  v[3].color = color;

  // UVマップ
  TextureSize size = texture_manager_->GetSizeById(texture_id);

  float u0 = uv.position.x / static_cast<float>(size.width);
  float v0 = uv.position.y / static_cast<float>(size.height);
  float u1 = (uv.position.x + uv.size.x) / static_cast<float>(size.width);
  float v1 = (uv.position.x + uv.size.y) / static_cast<float>(size.height);

  v[0].uv = {u0, v0};
  v[1].uv = {u1, v0};
  v[2].uv = {u0, v1};
  v[3].uv = {u1, v1};

  // 頂点バッファのロックを解除
  device_context_->Unmap(vertex_buffer_.Get(), 0);

  // 頂点バッファを描画パイプラインに設定
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  device_context_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);

  // 頂点シェーダーに変換行列を設定
  shader_manager_->SetProjectionMatrix(
    DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1280.0f, 720.0f, 0.0f, 0.0f, 1.0f));

  DirectX::XMMATRIX mat = MakeTransformMatrix(transform);

  shader_manager_->SetWorldMatrix(mat);

  // プリミティブトポロジ設定
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // ポリゴン描画命令発行
  device_context_->Draw(vertex_num_, 0);
}
