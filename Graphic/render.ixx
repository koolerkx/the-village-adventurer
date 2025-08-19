module;
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

export module graphic.render;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

import std;
import graphic.utils.types;
import graphic.shader;
import graphic.utils.color;
import graphic.texture;
import graphic.utils.font;

// 頂点構造体
export struct Vertex {
  POSITION position; // 頂点座標
  COLOR color;       // 色
  TEXCOORD uv;       // テクスチャー
};

struct InstanceData {
  DirectX::XMFLOAT2 pos;            // (x, y)
  DirectX::XMFLOAT2 size;           // (w, h)
  DirectX::XMFLOAT4 uv;             // (u0, v0, u1, v1)
  float radian;                     // float(rad)
  DirectX::XMFLOAT2 rotation_pivot; // (x, y)
  DirectX::XMFLOAT4 color;          // (r,g,b,a)
};

export class Renderer {
private:
  ComPtr<ID3D11Device> device_ = nullptr;
  ComPtr<ID3D11DeviceContext> device_context_ = nullptr;
  ShaderManager* shader_manager_ = nullptr;
  TextureManager* texture_manager_ = nullptr;

  ComPtr<ID3D11Buffer> vertex_buffer_ = nullptr;      // 頂点バッファ
  ComPtr<ID3D11Buffer> line_vertex_buffer_ = nullptr; // ライン頂点バッファ

  size_t rects_buffer_can_store_ = 0;                 // max = 2^32 = ~4.29E9
  ComPtr<ID3D11Buffer> rect_vertex_buffer_ = nullptr; // レクト頂点バッファ
  ComPtr<ID3D11Buffer> rect_index_buffer_;
  DXGI_FORMAT rect_index_format_ = DXGI_FORMAT_R32_UINT;

  // instance draw
  size_t instance_buffer_can_store_ = 0;                  // max = 2^32 = ~4.29E9
  ComPtr<ID3D11Buffer> instance_vertex_buffer_ = nullptr; // レクト頂点バッファ
  ComPtr<ID3D11Buffer> instance_index_buffer_;
  ComPtr<ID3D11Buffer> instance_quad_buffer_;
  DXGI_FORMAT instance_index_format_ = DXGI_FORMAT_R16_UINT;

  int vertex_num_ = 0;

  SIZE window_size_;

  static DirectX::XMMATRIX MakeProjectMatrix(SIZE window_size, CameraProps camera_props = {},
                                             bool is_half_pixel_offset_correction = false);
  static DirectX::XMMATRIX MakeTransformMatrix(const Transform& transform);

  DirectX::XMMATRIX mat_ortho_;

  void CreateRectBuffer(size_t max_rect_num);
  void CreateInstanceBuffer(size_t max_instance_num);

public:
  Renderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
           ShaderManager* shader_manager,
           TextureManager* texture_manager,
           SIZE window_size,
           int vertex_num = 4);

  void Draw(const Transform& transform, const COLOR& color);
  void DrawSprite(const FixedPoolIndexType texture_id,
                  const Transform& transform, const UV& uv,
                  const COLOR& color);

  void DrawSprite(RenderItem render_item, CameraProps camera_props = {});

  void DrawLine(const POSITION& start, const POSITION& end, const COLOR& color);
  // Indexed Draw
  void DrawLines(const std::span<Line> lines,
                 CameraProps camera_props = {},
                 bool is_half_pixel_offset_correction = false);
  void DrawRects(const std::span<Rect> rects,
                 CameraProps camera_props = {},
                 bool is_half_pixel_offset_correction = false);

  // Indexed Draw
  void DrawBox(Rect rect, CameraProps camera_props = {},
               bool is_half_pixel_offset_correction = false);
  void DrawBoxes(const std::span<const Rect> boxes,
                 CameraProps camera_props = {},
                 bool is_half_pixel_offset_correction = false);

  // Instanced Indexed Draw
  void DrawFont(const std::wstring& str, std::wstring font_key, Transform transform, StringSpriteProps props);

  // Instanced Indexed Draw
  void DrawSpritesInstanced(std::span<RenderInstanceItem> render_items,
                            FixedPoolIndexType texture_id,
                            CameraProps camera_props = {},
                            bool is_half_pixel_offset_correction = false);
};
