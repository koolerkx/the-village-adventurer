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

std::vector<CharSpriteMetadata> Font::MakeStringMetadata(const std::wstring& str) {
  std::vector<CharSpriteMetadata> result;
  for (wchar_t wc : str) {
    if (wc == L'\r') {
      continue; // ignore \r
    }
    if (wc == L'\n') {
      // insert empty object for new line `\n` (id=10)
      result.push_back({10, 0, 0, 0, 0, 0, 0, 0});
      continue;
    }

    unsigned int code = static_cast<unsigned int>(wc);
    auto it = charMap.find(code);
    if (it != charMap.end()) {
      result.push_back(it->second);
    }
    else {
      // missing char using `?` (id=63)
      auto q_it = charMap.find(63);
      if (q_it != charMap.end()) {
        result.push_back(q_it->second);
      }
      else {
        // fallback to space (id=32)
        result.push_back({0, 0, 0, 0, 0, 0, 0, 0});
        std::cerr << "Missing '?' in font for code: " << code << std::endl;
      }
    }
  }
  return result;
}

TextSize Font::GetStringSize(const std::wstring& str, float scale) {
  if (str.empty()) return {0.0f, 0.0f};

  float min_x = 0.0f, min_y = 0.0f;
  float max_x = 0.0f, max_y = 0.0f;
  bool has_char = false;

  float cursor_x = 0.0f;
  float cursor_y = 0.0f;
  const float scaled_line_height = static_cast<float>(line_height_) * scale;

  for (wchar_t wc : str) {
    if (wc == L'\r') continue;

    if (wc == L'\n') {
      cursor_x = 0.0f;
      cursor_y += scaled_line_height; // 下一行
      continue;
    }

    const unsigned int code = static_cast<unsigned int>(wc);
    auto it = charMap.find(code);
    if (it == charMap.end()) {
      auto sp = charMap.find(32u); // space
      if (sp != charMap.end()) {
        cursor_x += static_cast<float>(sp->second.xAdvance) * scale;
      }
      continue;
    }

    const auto& meta = it->second;

    const float left = cursor_x + static_cast<float>(meta.xOffset) * scale;
    const float top = cursor_y + static_cast<float>(meta.yOffset) * scale;
    const float right = left + static_cast<float>(meta.width) * scale;
    const float bottom = top + static_cast<float>(meta.height) * scale;

    if (!has_char) {
      min_x = left;
      max_x = right;
      min_y = top;
      max_y = bottom;
      has_char = true;
    }
    else {
      if (left < min_x) min_x = left;
      if (right > max_x) max_x = right;
      if (top < min_y) min_y = top;
      if (bottom > max_y) max_y = bottom;
    }

    cursor_x += static_cast<float>(meta.xAdvance) * scale;
  }

  if (!has_char) return {0.0f, 0.0f};

  return {max_x - min_x, max_y - min_y};
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
