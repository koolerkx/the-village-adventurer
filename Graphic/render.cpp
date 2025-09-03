module;
#include <d3d11.h>
#include <DirectXMath.h>
#include <cassert>
#include <WinString.h>

module graphic.render;

import graphic.texture;
import graphic.utils.font;
import graphic.utils.color;

constexpr int LINE_VERTEX_NUM = 65535;

Renderer::Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
                   ShaderManager* shader_manager,
                   TextureManager* texture_manager,
                   SIZE window_size,
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
  line_buff_desc.ByteWidth = sizeof(Vertex) * LINE_VERTEX_NUM;
  line_buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  line_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  device_->CreateBuffer(&line_buff_desc, NULL, line_vertex_buffer_.GetAddressOf());

  CreateRectBuffer(1024);
  CreateInstanceBuffer(1024);

  window_size_ = window_size;

  mat_ortho_ =
    DirectX::XMMatrixOrthographicOffCenterLH(0.0f,
                                             static_cast<float>(window_size_.cx), static_cast<float>(window_size_.cy),
                                             0.0f, 0.0f, 1.0f);
}

void Renderer::CreateRectBuffer(const size_t max_rect_num) {
  if (max_rect_num < rects_buffer_can_store_) return;
  rects_buffer_can_store_ = max_rect_num;

  // Vertex Buffer
  D3D11_BUFFER_DESC rect_buff_desc = {};
  rect_buff_desc.Usage = D3D11_USAGE_DYNAMIC;
  rect_buff_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * max_rect_num * 4);
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

void Renderer::CreateInstanceBuffer(const size_t max_instance_num) {
  if (max_instance_num < instance_buffer_can_store_) return;
  instance_buffer_can_store_ = max_instance_num;

  Vertex unitQuad[4] = {
    {{-0.5f, -0.5f, 0.f}, color::white, {0.f, 0.f}},
    {{+0.5f, -0.5f, 0.f}, color::white, {1.f, 0.f}},
    {{-0.5f, +0.5f, 0.f}, color::white, {0.f, 1.f}},
    {{+0.5f, +0.5f, 0.f}, color::white, {1.f, 1.f}},
  };

  D3D11_BUFFER_DESC vbDesc{};
  vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
  vbDesc.ByteWidth = sizeof(unitQuad);
  vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  D3D11_SUBRESOURCE_DATA vbData{unitQuad};
  device_->CreateBuffer(&vbDesc, &vbData, instance_quad_buffer_.GetAddressOf());

  constexpr uint16_t idx[6] = {0, 1, 2, 2, 1, 3};
  D3D11_BUFFER_DESC ibDesc{};
  ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
  ibDesc.ByteWidth = sizeof(idx);
  ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  D3D11_SUBRESOURCE_DATA ibData{idx};
  device_->CreateBuffer(&ibDesc, &ibData, instance_index_buffer_.GetAddressOf());

  // VB1: instance buffer（動態，?幀更新）
  D3D11_BUFFER_DESC instDesc{};
  instDesc.Usage = D3D11_USAGE_DYNAMIC;
  instDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  instDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  instDesc.ByteWidth = static_cast<UINT>(max_instance_num * sizeof(InstanceData));
  device_->CreateBuffer(&instDesc, nullptr, instance_vertex_buffer_.GetAddressOf());
}

DirectX::XMMATRIX Renderer::MakeProjectMatrix(SIZE window_size, CameraProps camera_props,
                                              bool is_half_pixel_offset_correction) {
  const float zoom = camera_props.zoom;
  const float width = static_cast<float>(window_size.cx);
  const float height = static_cast<float>(window_size.cy);

  // half pixel offset correction
  const float half_pixel_x = is_half_pixel_offset_correction ? 0.5f * (2.0f / width) : 0;
  const float half_pixel_y = is_half_pixel_offset_correction ? 0.5f * (2.0f / height) : 0;

  if (camera_props.algin_pivot == AlginPivot::CENTER_CENTER) {
    const float half_width = width / (2.0f * zoom);
    const float half_height = height / (2.0f * zoom);

    return
      DirectX::XMMatrixTranslation(-half_pixel_x, +half_pixel_y, 0.0f) *
      DirectX::XMMatrixOrthographicOffCenterLH(
        camera_props.position.x - half_width,
        camera_props.position.x + half_width,
        camera_props.position.y + half_height,
        camera_props.position.y - half_height,
        0.0f, 1.0f
      );
  }

  return
    DirectX::XMMatrixTranslation(-half_pixel_x, +half_pixel_y, 0.0f) *
    DirectX::XMMatrixOrthographicOffCenterLH(
      camera_props.position.x,
      camera_props.position.x + width / zoom,
      camera_props.position.y + height / zoom,
      camera_props.position.y,
      0.0f, 1.0f
    );
}

DirectX::XMMATRIX Renderer::MakeTransformMatrix(const Transform& transform) {
  using namespace DirectX;
  return XMMatrixTransformation2D(
    XMVectorSet(0, 0, 0, 0),                                                                       // 拡大縮小ピボットポイント
    0.0f,                                                                                          // 拡大縮小軸
    XMVectorSet(transform.size.x * transform.scale.x, transform.size.y * transform.scale.y, 0, 0), // 拡大縮小
    XMVectorSet(transform.rotation_pivot.x, transform.rotation_pivot.y, 0, 0),                     // 回転ピボットポイント
    transform.rotation_radian,                                                                     // 回転角度
    XMVectorSet(transform.position_anchor.x + transform.position.x + transform.size.x * transform.scale.x / 2,
                transform.position_anchor.y + transform.position.y + transform.size.y * transform.scale.y / 2, 0,
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
  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_));

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
  float v1 = (uv.position.y + uv.size.y) / static_cast<float>(size.height);

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
  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_));

  DirectX::XMMATRIX mat = MakeTransformMatrix(transform);

  shader_manager_->SetWorldMatrix(mat);

  // プリミティブトポロジ設定
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // ポリゴン描画命令発行
  device_context_->Draw(vertex_num_, 0);
}

void Renderer::DrawSprite(RenderItem render_item, CameraProps camera_props) {
  texture_manager_->SetShaderById(render_item.texture_id);
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

  v[0].color = render_item.color;
  v[1].color = render_item.color;
  v[2].color = render_item.color;
  v[3].color = render_item.color;

  // UVマップ
  TextureSize size = texture_manager_->GetSizeById(render_item.texture_id);

  float u0 = render_item.uv.position.x / static_cast<float>(size.width);
  float v0 = render_item.uv.position.y / static_cast<float>(size.height);
  float u1 = (render_item.uv.position.x + render_item.uv.size.x) / static_cast<float>(size.width);
  float v1 = (render_item.uv.position.y + render_item.uv.size.y) / static_cast<float>(size.height);

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
  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_, camera_props));

  DirectX::XMMATRIX mat = MakeTransformMatrix(render_item.transform);

  shader_manager_->SetWorldMatrix(mat);

  // プリミティブトポロジ設定
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // ポリゴン描画命令発行
  device_context_->Draw(vertex_num_, 0);
}

void Renderer::DrawLineCircle(POSITION center, float radius, const COLOR& color, CameraProps camera_props) {
  RenderItem item = {
    .texture_id = texture_manager_->Load(L"assets/circle.png"),
    // TODO: handle exception, debug use should not block game running
    .transform = {
      .position = {center.x - radius, center.y - radius, 0},
      .size = {radius * 2, radius * 2}
    },
    .uv = {{0, 0}, {64, 64}},
    .color = color
  };

  DrawSprite(item, camera_props);
}

void Renderer::DrawLine(const POSITION& start, const POSITION& end, const COLOR& color) {
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
  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_));

  shader_manager_->SetWorldMatrix(DirectX::XMMatrixIdentity());

  // プリミティブトポロジ設定
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  // ポリゴン描画命令発行
  device_context_->Draw(vertex_num_, 0);
}

void Renderer::DrawLines(const std::span<Line> lines,
                         CameraProps camera_props,
                         bool is_half_pixel_offset_correction) {
  if (lines.empty()) return;

  const std::wstring texture_filename = L"assets/block_white.png";
  const FixedPoolIndexType texture_id = texture_manager_->Load(texture_filename);

  texture_manager_->SetShaderById(texture_id);

  shader_manager_->Begin();

  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_, camera_props, is_half_pixel_offset_correction));
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

void Renderer::DrawRects(const std::span<Rect> rects,
                         CameraProps camera_props, bool is_half_pixel_offset_correction) {
  if (rects.empty()) return;

  if (rects.size() > rects_buffer_can_store_) {
    CreateRectBuffer(rects_buffer_can_store_ * 2);
  }

  const std::wstring texture_filename = L"assets/block_white.png";
  const FixedPoolIndexType texture_id = texture_manager_->Load(texture_filename);

  texture_manager_->SetShaderById(texture_id);

  shader_manager_->Begin();

  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_, camera_props, is_half_pixel_offset_correction));
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

void Renderer::DrawBox(Rect rect, CameraProps camera_props, bool is_half_pixel_offset_correction) {
  POSITION left_top = rect.left_top;
  POSITION right_top = {rect.right_bottom.x, rect.left_top.y, 0};
  POSITION left_bottom = {rect.left_top.x, rect.right_bottom.y, 0};
  POSITION right_bottom = rect.right_bottom;

  std::array<Line, 4> lines = {
    Line{left_top, right_top, rect.color},
    Line{right_top, right_bottom, rect.color},
    Line{left_bottom, right_bottom, rect.color},
    Line{left_top, left_bottom, rect.color},
  };

  DrawLines(lines, camera_props, is_half_pixel_offset_correction);
}

void Renderer::DrawBoxes(const std::span<const Rect> boxes,
                         CameraProps camera_props,
                         bool is_half_pixel_offset_correction) {
  std::vector<Line> lines;
  lines.reserve(boxes.size() * 4);

  for (auto& box : boxes) {
    lines.push_back({box.left_top, {box.right_bottom.x, box.left_top.y, 0}, box.color});
    lines.push_back({{box.right_bottom.x, box.left_top.y, 0}, box.right_bottom, box.color});
    lines.push_back({{box.left_top.x, box.right_bottom.y, 0}, box.right_bottom, box.color});
    lines.push_back({box.left_top, {box.left_top.x, box.right_bottom.y, 0}, box.color});
  }

  DrawLines(lines, camera_props, is_half_pixel_offset_correction);
}


void Renderer::DrawPolygon(const std::array<POSITION, 4>& points,
                           COLOR color,
                           CameraProps camera_props,
                           bool is_half_pixel_offset_correction) {
  std::vector<Line> lines;
  lines.reserve(4);

  POSITION left_top = points[0];
  POSITION right_top = points[1];
  POSITION right_bottom = points[2];
  POSITION left_bottom = points[3];

  lines.push_back({left_top, right_top, color});
  lines.push_back({right_top, right_bottom, color});
  lines.push_back({right_bottom, left_bottom, color});
  lines.push_back({left_bottom, left_top, color});

  DrawLines(lines, camera_props, is_half_pixel_offset_correction);
}

void Renderer::DrawFont(const std::wstring& str, std::wstring font_key, Transform transform, StringSpriteProps props,
                        CameraProps camera_props) {
  Font* font = Font::GetFont(font_key);

  if (props.is_draw_rect) {
    StringSpriteSize size = font->GetStringSize(str, transform, props);
    std::array<Rect, 1> ary({
      Rect{
        transform.position,
        {transform.position.x + size.width, transform.position.y + size.height, 0.0f},
        color::setOpacity(color::grey600, 0.2f)
      }
    });

    DrawRects(ary, camera_props);
  }

  std::vector<RenderInstanceItem> items = font->MakeStringRenderInstanceItems(str, transform, props);
  std::span<RenderInstanceItem> items_span = std::span(items.data(), items.size());
  DrawSpritesInstanced(items_span, font->GetTextureId(), camera_props);
}

void Renderer::DrawSpritesInstanced(const std::span<RenderInstanceItem> render_items,
                                    FixedPoolIndexType texture_id,
                                    CameraProps camera_props, bool is_half_pixel_offset_correction) {
  if (render_items.empty()) return;
  if (render_items.size() > instance_buffer_can_store_) {
    CreateInstanceBuffer(instance_buffer_can_store_ * 2);
  }

  texture_manager_->SetShaderById(texture_id);

  shader_manager_->Begin(VertexShaderType::Instance);

  shader_manager_->SetProjectionMatrix(MakeProjectMatrix(window_size_, camera_props, is_half_pixel_offset_correction));
  shader_manager_->SetWorldMatrix(DirectX::XMMatrixIdentity()); // FIXME

  D3D11_MAPPED_SUBRESOURCE msr{};
  device_context_->Map(instance_vertex_buffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
  auto* instances = static_cast<InstanceData*>(msr.pData);

  for (size_t i = 0; i < render_items.size(); ++i) {
    const auto& it = render_items[i];

    { // pos, size
      const auto& [x, y, _] = it.transform.position;
      const auto& [scale_x, scale_y] = it.transform.scale;
      const auto& [w, h] = it.transform.size;
      auto& anchor_x = it.transform.position_anchor.x;
      auto& anchor_y = it.transform.position_anchor.y;

      instances[i].pos = {anchor_x + x, anchor_y + y};
      instances[i].size = {w * scale_x, h * scale_y};
    }
    { // uvRect（u0,v0,u1,v1）
      const auto& [uv_x, uv_y] = it.uv.position;
      const auto& [uv_w, uv_h] = it.uv.size;
      const auto& [size_x, size_y] = texture_manager_->GetSizeById(texture_id);

      if (is_half_pixel_offset_correction) {
        const float u0 = (uv_x + 0.5f) / static_cast<float>(size_x);
        const float v0 = (uv_y + 0.5f) / static_cast<float>(size_y);
        const float u1 = (uv_x + uv_w - 0.5f) / static_cast<float>(size_x);
        const float v1 = (uv_y + uv_h - 0.5f) / static_cast<float>(size_y);

        instances[i].uv = {u0, v0, u1, v1};
      }
      else {
        const float u0 = uv_x / static_cast<float>(size_x);
        const float v0 = uv_y / static_cast<float>(size_y);
        const float u1 = (uv_x + uv_w) / static_cast<float>(size_x);
        const float v1 = (uv_y + uv_h) / static_cast<float>(size_y);

        instances[i].uv = {u0, v0, u1, v1};
      }
    }
    {
      auto& [rotation_x, rotation_y, _] = it.transform.rotation_pivot;
      auto& rad = it.transform.rotation_radian;
      instances[i].rotation_pivot = {rotation_x, rotation_y};
      instances[i].radian = rad;
    }
    {
      instances[i].color = it.color;
    }
  }
  device_context_->Unmap(instance_vertex_buffer_.Get(), 0);

  ID3D11Buffer* bufs[2] = {instance_quad_buffer_.Get(), instance_vertex_buffer_.Get()};
  UINT strides[2] = {sizeof(Vertex), sizeof(InstanceData)};
  UINT offsets[2] = {0, 0};
  device_context_->IASetVertexBuffers(0, 2, bufs, strides, offsets);

  device_context_->IASetIndexBuffer(instance_index_buffer_.Get(), instance_index_format_, 0);
  device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  constexpr UINT index_count_per_quad = 6;
  device_context_->DrawIndexedInstanced(index_count_per_quad,
                                        static_cast<UINT>(render_items.size()),
                                        0, 0, 0);
}

void Renderer::SetScissorRect(float left, float top, float right, float bottom) const {
  D3D11_RECT scissor_rect = {
    .left = static_cast<LONG>(left),
    .top = static_cast<LONG>(top),
    .right = static_cast<LONG>(right),
    .bottom = static_cast<LONG>(bottom)
  };
  device_context_->RSSetScissorRects(1, &scissor_rect);
}

void Renderer::ResetScissorRect() const {
  D3D11_RECT scissor_rect = {
    .left = 0,
    .top = 0,
    .right = window_size_.cx,
    .bottom = window_size_.cy
  };
  device_context_->RSSetScissorRects(1, &scissor_rect);
}
