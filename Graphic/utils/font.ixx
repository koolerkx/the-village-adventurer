module;

export module graphic.utils.font;

import std;
import graphic.texture;
import graphic.utils.fixed_pool;
import graphic.utils.types;

struct CharSpriteMetadata {
  unsigned int id = 32u; // unicode id
  unsigned short x = 0u; // u
  unsigned short y = 0u; // v
  unsigned short width = 0u;
  unsigned short height = 0u;
  short xOffset = 0;
  short yOffset = 0;
  short xAdvance = 0; // for proportional font
};


export enum class DefinedFont: unsigned short {
  FUSION_PIXEL_FONT_DEBUG,
};

export std::map<DefinedFont, std::pair<std::wstring, std::wstring>> defined_font_map({
  {
    DefinedFont::FUSION_PIXEL_FONT_DEBUG,
    std::pair<std::wstring, std::wstring>(L"assets\\fonts\\fusion-pixel-font-12px-monospaced-debug_0.png",
                                          L"assets\\fonts\\\\fusion-pixel-font-12px-monospaced-debug.fnt")
  }
});

export class Font {
private:
  std::wstring font_metadata_filename_;
  FixedPoolIndexType font_texture_id_;
  std::map<unsigned int, CharSpriteMetadata> charMap;
  unsigned short line_height_ = 0;
  unsigned short base_ = 0;

public:
  Font(FixedPoolIndexType texture_id, std::wstring fontMetadataFilename);
  std::vector<RenderInstanceItem> MakeStringRenderInstanceItems(const std::wstring& str, Transform transform,
                                                                StringSpriteProps props);
  StringSpriteSize GetStringSize(
    const std::wstring& str,
    const Transform& transform,
    const StringSpriteProps& props
  );

  FixedPoolIndexType GetTextureId() const { return font_texture_id_; }

  // Font Factory
  inline static std::map<std::wstring, std::unique_ptr<Font>> font_map_ = {};
  static std::wstring MakeFontGetKey(const std::wstring& font_filename,
                                     std::wstring fontMetadataFilename,
                                     TextureManager* texture_manager
  );
  static Font* GetFont(const std::wstring& font_filename);
};
