module;
#include <cassert>

module graphic.utils.font;

import std;

Font::Font(FixedPoolIndexType texture_id, std::wstring fontMetadataFilename)
  : font_metadata_filename_(fontMetadataFilename),
    font_texture_id_(texture_id) {
  // decode font metadata (.fnt)
  std::ifstream in(fontMetadataFilename);
  if (!in) {
    assert(false);
  }

  std::string line;
  while (std::getline(in, line)) {
    std::stringstream ss(line);
    std::string token;
    ss >> token;

    if (token == "common") {
      while (ss >> token) {
        if (token.starts_with("lineHeight=")) {
          line_height_ = static_cast<unsigned short>(std::stoul(token.substr(11)));
        }
        else if (token.starts_with("base=")) {
          base_ = static_cast<unsigned short>(std::stoul(token.substr(5)));
        }
        // ignroe scaleW, scaleH, pages, packed, chnl（assume single page）
      }
    }
    else if (token == "page") {
      // while (ss >> token) {
      //   if (token.starts_with("file=\"")) {
      //     font_filename = token.substr(6, token.size() - 7); // remove `file="` and `"`
      //   }
      //   // ignore id
      // }
      continue;
    }
    else if (token == "char") {
      CharSpriteMetadata metadata;

      while (ss >> token) {
        if (token.starts_with("id=")) {
          metadata.id = std::stoul(token.substr(3));
        }
        else if (token.starts_with("x=")) {
          metadata.x = static_cast<unsigned short>(std::stoul(token.substr(2)));
        }
        else if (token.starts_with("y=")) {
          metadata.y = static_cast<unsigned short>(std::stoul(token.substr(2)));
        }
        else if (token.starts_with("width=")) {
          metadata.width = static_cast<unsigned short>(std::stoul(token.substr(6)));
        }
        else if (token.starts_with("height=")) {
          metadata.height = static_cast<unsigned short>(std::stoul(token.substr(7)));
        }
        else if (token.starts_with("xoffset=")) {
          metadata.xOffset = static_cast<short>(std::stol(token.substr(8)));
        }
        else if (token.starts_with("yoffset=")) {
          metadata.yOffset = static_cast<short>(std::stol(token.substr(8)));
        }
        else if (token.starts_with("xadvance=")) {
          metadata.xAdvance = static_cast<short>(std::stol(token.substr(9)));
        }
        // ignore page, chnl
      }

      if (metadata.id != 0) {
        charMap[metadata.id] = metadata;
      }
    }
    // ignore info, chars count etc
  }
}

// transform scale handled, caller should reset scale to 1
std::vector<RenderInstanceItem> Font::MakeStringRenderInstanceItems(
  const std::wstring& str, Transform transform, StringSpriteProps props
) {
  std::vector<RenderInstanceItem> items;
  items.reserve(str.size());

  float cursor_x = 0.0f;
  float cursor_y = 0.0f;

  const float sx = (props.pixel_size / line_height_) * transform.scale.x;
  const float sy = (props.pixel_size / line_height_) * transform.scale.y;

  const float line_height = props.line_height > 0 ? props.line_height : static_cast<float>(line_height_);
  const float line_advance = (static_cast<float>(line_height) + props.line_spacing) * sy;

  for (wchar_t wc : str) {
    if (wc == L'\r') {
      continue; // ignore \r
    }
    if (wc == L'\n') {
      // insert empty object for new line `\n` (id=10)
      cursor_x = 0.0f;
      cursor_y += line_advance;
      continue;
    }

    unsigned int code = static_cast<unsigned int>(wc);
    const CharSpriteMetadata* meta = nullptr;

    if (auto it = charMap.find(code); it != charMap.end()) {
      meta = &it->second;
    }
    else if (auto q = charMap.find(63u); q != charMap.end()) { // '?'
      meta = &q->second;
    }
    else if (auto sp = charMap.find(32u); sp != charMap.end()) { // space
      meta = &sp->second;
    }
    else {
      continue;
    }

    const float gx = transform.position.x + cursor_x + static_cast<float>(meta->xOffset) * sx;
    const float gy = transform.position.y + cursor_y + static_cast<float>(meta->yOffset) * sy;
    const float gz = transform.position.z;

    Transform gt = transform;
    gt.position = {gx, gy, gz};
    gt.size = {static_cast<float>(meta->width * sx), static_cast<float>(meta->height * sy)};

    const float u0 = meta->x;
    const float v0 = meta->y;
    const float u1 = meta->width;
    const float v1 = meta->height;

    UV uv{};
    uv.position = {u0, v0};
    uv.size = {u1, v1};

    RenderInstanceItem item{};
    item.transform = gt;
    item.transform.scale.x = 1; // scale handled
    item.transform.scale.y = 1; // scale handled
    item.uv = uv;
    item.color = props.color;

    items.push_back(item);

    cursor_x += (static_cast<float>(meta->xAdvance) + props.letter_spacing) * sx;
  }
  return items;
}

StringSpriteSize Font::GetStringSize(
  const std::wstring& str,
  const Transform& transform,
  const StringSpriteProps& props
) {
  if (str.empty()) return {0.0f, 0.0f};

  const float sx = (props.pixel_size / line_height_) * transform.scale.x;
  const float sy = (props.pixel_size / line_height_) * transform.scale.y;

  const float line_height = props.line_height > 0 ? props.line_height : static_cast<float>(line_height_);
  const float line_advance = (static_cast<float>(line_height) + props.line_spacing) * sy;

  float min_x = 0.0f, min_y = 0.0f;
  float max_x = 0.0f, max_y = 0.0f;
  bool has_bbox = false;

  float cursor_x = 0.0f;
  float cursor_y = 0.0f;

  auto get_meta = [&](unsigned int code) -> const CharSpriteMetadata* {
    if (auto it = charMap.find(code); it != charMap.end()) return &it->second;
    if (auto q = charMap.find(63u); q != charMap.end()) return &q->second;    // '?'
    if (auto sp = charMap.find(32u); sp != charMap.end()) return &sp->second; // ' '
    return nullptr;
  };

  for (wchar_t wc : str) {
    if (wc == L'\r') continue;

    if (wc == L'\n') {
      cursor_x = 0.0f;
      cursor_y += line_advance;
      continue;
    }

    const unsigned int code = static_cast<unsigned int>(wc);
    const CharSpriteMetadata* meta = get_meta(code);
    if (!meta) {
      continue;
    }

    const float left = cursor_x + static_cast<float>(meta->xOffset) * sx;
    const float top = cursor_y + static_cast<float>(meta->yOffset) * sy;
    const float right = left + static_cast<float>(meta->width) * sx;
    const float bottom = top + static_cast<float>(meta->height) * sy;

    if (!has_bbox) {
      min_x = left;
      max_x = right;
      min_y = top;
      max_y = bottom;
      has_bbox = true;
    }
    else {
      if (left < min_x) min_x = left;
      if (right > max_x) max_x = right;
      if (top < min_y) min_y = top;
      if (bottom > max_y) max_y = bottom;
    }

    cursor_x += (static_cast<float>(meta->xAdvance) + props.letter_spacing) * sx;
  }

  const float baseline_bottom = cursor_y + static_cast<float>(line_height_) * sy;

  if (!has_bbox) {
    return {0.0f, baseline_bottom};
  }

  const float bbox_w = max_x - min_x;
  const float bbox_h = max_y - min_y;

  const float height = std::max(bbox_h, baseline_bottom - min_y);
  return {bbox_w, height};
}

std::wstring Font::MakeFontGetKey(const std::wstring& font_filename, std::wstring fontMetadataFilename,
                                  TextureManager* texture_manager) {
  FixedPoolIndexType texture_id = texture_manager->Load(font_filename);

  if (font_map_.contains(font_filename)) return font_filename;

  auto font = std::make_unique<Font>(texture_id, fontMetadataFilename);

  font_map_[font_filename] = std::move(font);
  return font_filename;
}

Font* Font::GetFont(const std::wstring& font_filename) {
  if (!font_map_.contains(font_filename)) {
    assert(false);
  }
  return font_map_[font_filename].get();
}
