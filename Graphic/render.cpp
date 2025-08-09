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
                   SIZE, // FIXME, load window size from file
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

  D3D11_BUFFER_DESC line_buff_desc = {};
  line_buff_desc.Usage = D3D11_USAGE_DYNAMIC;
  line_buff_desc.ByteWidth = sizeof(Vertex) * 6000; // todo: extract
  line_buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  line_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  device_->CreateBuffer(&line_buff_desc, NULL, line_vertex_buffer_.GetAddressOf());

  CreateRectBuffer(1024);
  
  // FIXME: load window size from config file
  mat_ortho_ =
    DirectX::XMMatrixOrthographicOffCenterLH(0.0f, 1280, 720, 0.0f, 0.0f, 1.0f);
}

void Renderer::CreateRectBuffer(size_t max_rect_num) {
  if (max_rect_num < rects_buffer_can_store_) return;

  // Vertex Buffer
  D3D11_BUFFER_DESC rect_buff_desc = {};
  rect_buff_desc.Usage = D3D11_USAGE_DYNAMIC;
  rect_buff_desc.ByteWidth = sizeof(Vertex) * max_rect_num * 4;
  rect_buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  rect_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  device_->CreateBuffer(&rect_buff_desc, NULL, rect_vertex_buffer_.GetAddressOf());

  // Index Buffer
  const size_t max_indices = max_rect_num * 4 + (max_rect_num - 1); // insert cut value between each rect
  D3D11_BUFFER_DESC bd{};
  bd.Usage = D3D11_USAGE_DYNAMIC;
  bd.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * max_indices);
  bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  rect_index_buffer_.Reset();
  device_->CreateBuffer(&bd, nullptr, rect_index_buffer_.GetAddressOf());
  rect_index_format_ = DXGI_FORMAT_R32_UINT; // cut value: 0xFFFFFFFF
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

void Renderer::DrawLineForDebugUse(const POSITION& start, const POSITION& end, const COLOR& color) {
  const std::wstring texture_filename = L"assets/block_white.png";
  const FixedPoolIndexType texture_id = texture_manager_->Load(texture_filename);

  texture_manager_->SetShaderById(texture_id);

  shader_manager_->Begin();

  // 頂点バッファをロックする
  D3D11_MAPPED_SUBRESOURCE msr;
  device_context_->Map(vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

  // 頂点バッファへの仮想ポインタを取得
  Vertex* v = static_cast<Vertex*>(msr.pData);

  // 画面の左上から右下に向かう線分を描画する
  v[0].position = {start.x, start.y, 0.0f}; // LT
  v[1].position = {end.x, end.y, 0.0f};     // RT

  v[0].color = color;
  v[1].color = color;

  v[0].uv = {0, 0};
  v[1].uv = {1, 1};

  // 頂点バッファのロックを解除
  device_context_->Unmap(vertex_buffer_.Get(), 0);

  // 頂点バッファを描画パイプラインに設定
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  device_context_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);

  // 頂点シェーダーに変換行列を設定
  shader_manager_->SetProjectionMatrix(mat_ortho_);

  shader_manager_->SetWorldMatrix(DirectX::XMMatrixIdentity());

  // プリミティブトポロジ設定
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  // ポリゴン描画命令発行
  device_context_->Draw(vertex_num_, 0);
}

void Renderer::DrawLinesForDebugUse(const std::span<Line> lines) {
  if (lines.empty()) return;

  const std::wstring texture_filename = L"assets/block_white.png";
  const FixedPoolIndexType texture_id = texture_manager_->Load(texture_filename);

  texture_manager_->SetShaderById(texture_id);

  shader_manager_->Begin();

  shader_manager_->SetProjectionMatrix(mat_ortho_);
  shader_manager_->SetWorldMatrix(DirectX::XMMatrixIdentity());

  D3D11_MAPPED_SUBRESOURCE msr{};
  device_context_->Map(line_vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
  Vertex* v = static_cast<Vertex*>(msr.pData);

  for (size_t i = 0; i < lines.size(); i++) {
    const auto& [start, end, color] = lines[i];
    const UINT base = static_cast<UINT>(i * 2);

    v[base + 0].position = {start.x, start.y, 0.0f};
    v[base + 1].position = {end.x, end.y, 0.0f};

    v[base + 0].color = color;
    v[base + 1].color = color;

    v[base + 0].uv = {0.0f, 0.0f};
    v[base + 1].uv = {1.0f, 1.0f};
  }

  device_context_->Unmap(line_vertex_buffer_.Get(), 0);

  UINT stride = sizeof(Vertex), offset = 0;
  ID3D11Buffer* vb = line_vertex_buffer_.Get();
  device_context_->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  device_context_->Draw(static_cast<UINT>(lines.size() * 2), 0);
}

void Renderer::DrawRectsForDebugUse(const std::span<Rect> rects) {
  if (rects.empty()) return;

  const std::wstring texture_filename = L"assets/block_white.png";
  const FixedPoolIndexType texture_id = texture_manager_->Load(texture_filename);

  texture_manager_->SetShaderById(texture_id);

  shader_manager_->Begin();

  shader_manager_->SetProjectionMatrix(mat_ortho_);
  shader_manager_->SetWorldMatrix(DirectX::XMMatrixIdentity());

  // Vertex Buffer
  {
    D3D11_MAPPED_SUBRESOURCE msr{};
    device_context_->Map(rect_vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    Vertex* v = static_cast<Vertex*>(msr.pData);

    for (size_t i = 0; i < rects.size(); i++) {
      const auto& [left_top, right_bottom, color] = rects[i];
      const UINT base = static_cast<UINT>(i * 4);

      v[base + 0].position = {left_top.x, left_top.y, 0.0f};         // LT
      v[base + 1].position = {right_bottom.x, left_top.y, 0.0f};     // RT
      v[base + 2].position = {left_top.x, right_bottom.y, 0.0f};     // LB
      v[base + 3].position = {right_bottom.x, right_bottom.y, 0.0f}; // RB

      v[base + 0].color = color;
      v[base + 1].color = color;
      v[base + 2].color = color;
      v[base + 3].color = color;

      v[base + 0].uv = {0, 0};
      v[base + 1].uv = {1, 0};
      v[base + 2].uv = {0, 1};
      v[base + 3].uv = {1, 1};
    }

    device_context_->Unmap(rect_vertex_buffer_.Get(), 0);
  }

  // Index Buffer
  const uint32_t CUT = 0xFFFFFFFFu; // DXGI_FORMAT_R32_UINT primitive-restart
  const size_t index_count = rects.size() * 4 + (rects.size() - 1);

  {
    D3D11_MAPPED_SUBRESOURCE msr{};
    device_context_->Map(rect_index_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    auto* idx = static_cast<uint32_t*>(msr.pData);

    size_t w = 0;
    for (uint32_t i = 0; i < static_cast<uint32_t>(rects.size()); ++i) {
      const uint32_t b = i * 4;
      idx[w++] = b + 0; // LT
      idx[w++] = b + 1; // RT
      idx[w++] = b + 2; // LB
      idx[w++] = b + 3; // RB

      if (i + 1 < rects.size()) {
        idx[w++] = CUT;
      }
    }
    device_context_->Unmap(rect_index_buffer_.Get(), 0);
  }


  UINT stride = sizeof(Vertex), offset = 0;
  ID3D11Buffer* vb = rect_vertex_buffer_.Get();
  device_context_->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
  device_context_->IASetIndexBuffer(rect_index_buffer_.Get(), rect_index_format_, 0);

  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  device_context_->DrawIndexed(static_cast<UINT>(index_count), 0, 0);
}
