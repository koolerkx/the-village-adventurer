/**
* @file shader_vertex_2d.hlsl
 * @brief 2D描画用頂点シェーダー
 * @author KOOLER FAN
 * @date 2025/06/10
 */

// 定数バッファ
cbuffer VS_CONSTANT_BUFFER: 

register (b0) {
  float4x4 proj;
};

cbuffer VS_CONSTANT_BUFFER: 

register (b1) {
  float4x4 world;
};

struct VS_INPUT {
  float4 posL  : POSITION0; // ローカル座標 
  float4 color : COLOR0;    // Color
  float2 uv    : TEXCOORD0;
};

struct VS_OUTPUT {
  float4 posH  : SV_POSITION; // 変換済み座標 
  float4 color : COLOR0;      // Color
  float2 uv    : TEXCOORD0;
};

// 頂点シェーダ
VS_OUTPUT main(VS_INPUT vs_in) {
  VS_OUTPUT vs_out;

  // 座標変換
  // float4x4 mtx = mul(world, proj); // 同じ意味で、合成行列
  // vs_out.posH = mul(vs_in.posL, mtx);
  vs_in.posL = mul(vs_in.posL, world);
  vs_out.posH = mul(vs_in.posL, proj);

  vs_out.color = vs_in.color;
  vs_out.uv = vs_in.uv;
  return vs_out;
}
