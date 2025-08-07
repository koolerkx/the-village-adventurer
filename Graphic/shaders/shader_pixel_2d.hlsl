/**
* @file shader_pixel_2d.hlsl
 * @brief 2D描画用ピクセルシェーダー
 * @author KOOLER FAN
 * @date 2025/06/10
 */

struct PS_INPUT {
  float4 posH : SV_POSITION;
  float4 color : COLOR0;
  float2 uv : TEXCOORD0;
};

Texture2D tex;     // テクスチャ
SamplerState samp; // テクスチャさんプラ

float4 main(PS_INPUT ps_in) : SV_TARGET {
  // return ps_in.color;
  return tex.Sample(samp, ps_in.uv) * ps_in.color;
}
