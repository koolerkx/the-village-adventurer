/**
 * @file swirl_pixel.hlsl
 * @brief 渦巻きエフェクト
 * @author KOOLER FAN
 * @date 2025/09/07
 */

cbuffer SwirlParams : register(b1) {
  float2 swirlCenter1;
  float swirlRadius1;
  float swirlTwists1;
  float2 swirlCenter2;
  float swirlRadius2;
  float swirlTwists2;
};

struct PS_INPUT {
  float4 posH : SV_POSITION;
  float4 color : COLOR0;
  float2 uv : TEXCOORD0;
};

Texture2D tex;     // テクスチャ
SamplerState samp; // テクスチャさんプラ

float2 swirl(float2 uv, float2 center, float radius, float strength) {
  float2 delta = uv - center;
  float dist = length(delta);

  if (dist < radius) {
    float percent = (radius - dist) / radius;
    float angle = percent * strength * 6.2831853;
    float s = sin(angle);
    float c = cos(angle);
    delta = float2(delta.x * c - delta.y * s,
                   delta.x * s + delta.y * c);
    uv = center + delta;
  }
  return uv;
}

float4 main(PS_INPUT ps_in) : SV_TARGET {
  float2 uv = ps_in.uv;

  uv = swirl(uv, swirlCenter1, swirlRadius1, swirlTwists1);
  uv = swirl(uv, swirlCenter2, swirlRadius2, swirlTwists2);

  return tex.Sample(samp, uv) * ps_in.color;
}
