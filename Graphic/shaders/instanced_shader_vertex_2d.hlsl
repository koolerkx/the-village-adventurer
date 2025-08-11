/**
 * @file instanced_shader_vertex_2d.hlsl
 * @brief vertex shader for instanced rendering
 *
 * The position anchor/pivot default to left top,
 * other anchor is expected to be set by the caller
 * Quad color and uv is ignored, use per-instance data
 *
 * @author Kooler Fan
 * @date 2025/08/11
 */
struct VS_INPUT {
  float3 posL : POSITION; // unit quad [-0.5..+0.5]
  float4 color : COLOR0;
  float2 uv : TEXCOORD0;

  // per-instance (slot 1)
  float2 iPos : TEXCOORD1;    // translate (x,y)
  float2 iScale : TEXCOORD2;  // scale (w,h)
  float4 iUVRect : TEXCOORD3; // uvRect(u0, v0, u1, v1)
  float1 iRad : TEXCOORD4;    // float(rad)
  float2 iPivot : TEXCOORD5;  // pivot(x, y)
  float4 iColor : TEXCOORD6;  // per-instance color
};

cbuffer CbProj : register(b0) {
  row_major float4x4 proj;
};

cbuffer CbWorld : register(b1) {
  row_major float4x4 world;
};

struct VS_OUTPUT {
  float4 posH : SV_Position;
  float4 color : COLOR0;
  float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT IN) {
  VS_OUTPUT OUT;

  // scaled
  float2 scaledXY = IN.posL.xy * IN.iScale;

  // rotation + rotation pivot
  scaledXY = scaledXY - IN.iPivot;
  float sin_rad, cos_rad;
  sincos(IN.iRad, sin_rad, cos_rad);
  float2 rotatedXY;
  rotatedXY.x = scaledXY.x * cos_rad - scaledXY.y * sin_rad;
  rotatedXY.y = scaledXY.x * sin_rad + scaledXY.y * cos_rad;
  rotatedXY = rotatedXY + IN.iPivot;

  // translation
  float2 worldXY = (rotatedXY + IN.iScale * 0.5) + IN.iPos;

  // World and Projection Matrix
  float4 posW = float4(worldXY, IN.posL.z, 1.0); // identity matrix expected
  float4 posV = mul(world, posW); // camera
  OUT.posH = mul(proj, posV);

  // UB
  OUT.uv = float2(
    lerp(IN.iUVRect.x, IN.iUVRect.z, IN.uv.x),
    lerp(IN.iUVRect.y, IN.iUVRect.w, IN.uv.y)
  );

  // ignore quad color, use per-instance color
  // OUT.color = IN.color * IN.iColor;
  OUT.color = IN.iColor;
  return OUT;
}
