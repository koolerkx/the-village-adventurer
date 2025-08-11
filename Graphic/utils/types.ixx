module;
#include <DirectXMath.h>

export module graphic.utils.types;
import std;
import graphic.utils.color;
import graphic.utils.fixed_pool;

export using POSITION = DirectX::XMFLOAT3;
export using UVPOSITION = DirectX::XMFLOAT2;
export using TEXCOORD = DirectX::XMFLOAT2;

export using SCALE = DirectX::XMFLOAT2;

export struct Transform {
  POSITION position = {0.0f, 0.0f, 0.0f};
  SCALE size = {100.0f, 100.0f};
  SCALE scale = {1.0f, 1.0f};
  float rotation_radian = 0.0f;
  POSITION rotation_pivot = {0.0f, 0.0f, 0.0f};
  POSITION position_anchor = {0.0f, 0.0f, 0.0f};
};

export struct UV {
  UVPOSITION position;
  SCALE size;
};

export struct Line {
  POSITION start;
  POSITION end;
  COLOR color;
};

export struct Rect {
  POSITION left_top;
  POSITION right_bottom;
  COLOR color;
};

export struct Box {
  POSITION left_top;
  POSITION right_bottom;
};

export struct RenderItem {
  FixedPoolIndexType texture_id;
  Transform transform;
  UV uv;
  COLOR color;
};

export struct RenderInstanceItem {
  Transform transform;
  UV uv;
  COLOR color;
};

export struct StringSpriteSize {
  float width, height;
};

export struct StringSpriteProps {
  float pixel_size = 12.0f;
  float letter_spacing = 0.0f;
  float line_height = -1.0f;
  float line_spacing = 0;
  COLOR color = color::white;
};
