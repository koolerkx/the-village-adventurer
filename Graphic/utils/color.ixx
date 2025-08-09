module;
#include <DirectXMath.h>

export module graphic.utils.color;

import std;
export using COLOR = DirectX::XMFLOAT4;

constexpr float hexToFloat(char hex1, char hex2) {
  int value = 0;
  if (hex1 >= '0' && hex1 <= '9') {
    value = (hex1 - '0') * 16;
  } else if (hex1 >= 'A' && hex1 <= 'F') {
    value = (hex1 - 'A' + 10) * 16;
  } else if (hex1 >= 'a' && hex1 <= 'f') {
    value = (hex1 - 'a' + 10) * 16;
  }
    
  if (hex2 >= '0' && hex2 <= '9') {
    value += (hex2 - '0');
  } else if (hex2 >= 'A' && hex2 <= 'F') {
    value += (hex2 - 'A' + 10);
  } else if (hex2 >= 'a' && hex2 <= 'f') {
    value += (hex2 - 'a' + 10);
  }

  return value / 255.0f;
}

constexpr DirectX::XMFLOAT4 toRGB(std::string_view hex, float alpha = 1.0f) {
  return DirectX::XMFLOAT4(
      hexToFloat(hex[1], hex[2]),
      hexToFloat(hex[3], hex[4]),
      hexToFloat(hex[5], hex[6]),
      alpha
  );
}

export namespace color {
  constexpr COLOR TransparentWhite = toRGB("#111111", 0.0f);
  constexpr COLOR TransparentBlack = toRGB("#000000", 0.0f);
  
  // HTML Color
  // https://htmlcolorcodes.com/color-names/
  // Red Color
  constexpr COLOR IndianRed = toRGB("#CD5C5C", 1.0f);
  constexpr COLOR LightCoral = toRGB("#F08080", 1.0f);
  constexpr COLOR Salmon = toRGB("#FA8072", 1.0f);
  constexpr COLOR DarkSalmon = toRGB("#E9967A", 1.0f);
  constexpr COLOR LightSalmon = toRGB("#FFA07A", 1.0f);
  constexpr COLOR Crimson = toRGB("#DC143C", 1.0f);
  constexpr COLOR Red = toRGB("#FF0000", 1.0f);
  constexpr COLOR FireBrick = toRGB("#B22222", 1.0f);
  constexpr COLOR DarkRed = toRGB("#8B0000", 1.0f);

  // Pink Color
  constexpr COLOR Pink = toRGB("#FFC0CB", 1.0f);
  constexpr COLOR LightPink = toRGB("#FFB6C1", 1.0f);
  constexpr COLOR HotPink = toRGB("#FF69B4", 1.0f);
  constexpr COLOR DeepPink = toRGB("#FF1493", 1.0f);
  constexpr COLOR MediumVioletRed = toRGB("#C71585", 1.0f);
  constexpr COLOR PaleVioletRed = toRGB("#DB7093", 1.0f);

  // Orange Color
  constexpr COLOR LightSalmonOrange = toRGB("#FFA07A", 1.0f); // Note: LightSalmon appears in both Red and Orange
  constexpr COLOR Coral = toRGB("#FF7F50", 1.0f);
  constexpr COLOR Tomato = toRGB("#FF6347", 1.0f);
  constexpr COLOR OrangeRed = toRGB("#FF4500", 1.0f);
  constexpr COLOR DarkOrange = toRGB("#FF8C00", 1.0f);
  constexpr COLOR Orange = toRGB("#FFA500", 1.0f);

  // Yellow Color
  constexpr COLOR Gold = toRGB("#FFD700", 1.0f);
  constexpr COLOR Yellow = toRGB("#FFFF00", 1.0f);
  constexpr COLOR LightYellow = toRGB("#FFFFE0", 1.0f);
  constexpr COLOR LemonChiffon = toRGB("#FFFACD", 1.0f);
  constexpr COLOR LightGoldenrodYellow = toRGB("#FAFAD2", 1.0f);
  constexpr COLOR PapayaWhip = toRGB("#FFEFD5", 1.0f);
  constexpr COLOR Moccasin = toRGB("#FFE4B5", 1.0f);
  constexpr COLOR PeachPuff = toRGB("#FFDAB9", 1.0f);
  constexpr COLOR PaleGoldenrod = toRGB("#EEE8AA", 1.0f);
  constexpr COLOR Khaki = toRGB("#F0E68C", 1.0f);
  constexpr COLOR DarkKhaki = toRGB("#BDB76B", 1.0f);

  // Purple Color
  constexpr COLOR Lavender = toRGB("#E6E6FA", 1.0f);
  constexpr COLOR Thistle = toRGB("#D8BFD8", 1.0f);
  constexpr COLOR Plum = toRGB("#DDA0DD", 1.0f);
  constexpr COLOR Violet = toRGB("#EE82EE", 1.0f);
  constexpr COLOR Orchid = toRGB("#DA70D6", 1.0f);
  constexpr COLOR Fuchsia = toRGB("#FF00FF", 1.0f);
  constexpr COLOR Magenta = toRGB("#FF00FF", 1.0f);
  constexpr COLOR MediumOrchid = toRGB("#BA55D3", 1.0f);
  constexpr COLOR MediumPurple = toRGB("#9370DB", 1.0f);
  constexpr COLOR RebeccaPurple = toRGB("#663399", 1.0f);
  constexpr COLOR BlueViolet = toRGB("#8A2BE2", 1.0f);
  constexpr COLOR DarkViolet = toRGB("#9400D3", 1.0f);
  constexpr COLOR DarkOrchid = toRGB("#9932CC", 1.0f);
  constexpr COLOR DarkMagenta = toRGB("#8B008B", 1.0f);
  constexpr COLOR Purple = toRGB("#800080", 1.0f);
  constexpr COLOR Indigo = toRGB("#4B0082", 1.0f);
  constexpr COLOR SlateBlue = toRGB("#6A5ACD", 1.0f);
  constexpr COLOR DarkSlateBlue = toRGB("#483D8B", 1.0f);
  constexpr COLOR MediumSlateBlue = toRGB("#7B68EE", 1.0f);

  // Green Color
  constexpr COLOR GreenYellow = toRGB("#ADFF2F", 1.0f);
  constexpr COLOR Chartreuse = toRGB("#7FFF00", 1.0f);
  constexpr COLOR LawnGreen = toRGB("#7CFC00", 1.0f);
  constexpr COLOR Lime = toRGB("#00FF00", 1.0f);
  constexpr COLOR LimeGreen = toRGB("#32CD32", 1.0f);
  constexpr COLOR PaleGreen = toRGB("#98FB98", 1.0f);
  constexpr COLOR LightGreen = toRGB("#90EE90", 1.0f);
  constexpr COLOR MediumSpringGreen = toRGB("#00FA9A", 1.0f);
  constexpr COLOR SpringGreen = toRGB("#00FF7F", 1.0f);
  constexpr COLOR MediumSeaGreen = toRGB("#3CB371", 1.0f);
  constexpr COLOR SeaGreen = toRGB("#2E8B57", 1.0f);
  constexpr COLOR ForestGreen = toRGB("#228B22", 1.0f);
  constexpr COLOR Green = toRGB("#008000", 1.0f);
  constexpr COLOR DarkGreen = toRGB("#006400", 1.0f);
  constexpr COLOR YellowGreen = toRGB("#9ACD32", 1.0f);
  constexpr COLOR OliveDrab = toRGB("#6B8E23", 1.0f);
  constexpr COLOR Olive = toRGB("#808000", 1.0f);
  constexpr COLOR DarkOliveGreen = toRGB("#556B2F", 1.0f);
  constexpr COLOR MediumAquamarine = toRGB("#66CDAA", 1.0f);
  constexpr COLOR DarkSeaGreen = toRGB("#8FBC8B", 1.0f);
  constexpr COLOR LightSeaGreen = toRGB("#20B2AA", 1.0f);
  constexpr COLOR DarkCyan = toRGB("#008B8B", 1.0f);
  constexpr COLOR Teal = toRGB("#008080", 1.0f);

  // Blue Color
  constexpr COLOR Aqua = toRGB("#00FFFF", 1.0f);
  constexpr COLOR Cyan = toRGB("#00FFFF", 1.0f);
  constexpr COLOR LightCyan = toRGB("#E0FFFF", 1.0f);
  constexpr COLOR PaleTurquoise = toRGB("#AFEEEE", 1.0f);
  constexpr COLOR Aquamarine = toRGB("#7FFFD4", 1.0f);
  constexpr COLOR Turquoise = toRGB("#40E0D0", 1.0f);
  constexpr COLOR MediumTurquoise = toRGB("#48D1CC", 1.0f);
  constexpr COLOR DarkTurquoise = toRGB("#00CED1", 1.0f);
  constexpr COLOR CadetBlue = toRGB("#5F9EA0", 1.0f);
  constexpr COLOR SteelBlue = toRGB("#4682B4", 1.0f);
  constexpr COLOR LightSteelBlue = toRGB("#B0C4DE", 1.0f);
  constexpr COLOR PowderBlue = toRGB("#B0E0E6", 1.0f);
  constexpr COLOR LightBlue = toRGB("#ADD8E6", 1.0f);
  constexpr COLOR SkyBlue = toRGB("#87CEEB", 1.0f);
  constexpr COLOR LightSkyBlue = toRGB("#87CEFA", 1.0f);
  constexpr COLOR DeepSkyBlue = toRGB("#00BFFF", 1.0f);
  constexpr COLOR DodgerBlue = toRGB("#1E90FF", 1.0f);
  constexpr COLOR CornflowerBlue = toRGB("#6495ED", 1.0f);
  constexpr COLOR MediumSlateBlueBlue = toRGB("#7B68EE", 1.0f); // Note: MediumSlateBlue appears in both Purple and Blue
  constexpr COLOR RoyalBlue = toRGB("#4169E1", 1.0f);
  constexpr COLOR Blue = toRGB("#0000FF", 1.0f);
  constexpr COLOR MediumBlue = toRGB("#0000CD", 1.0f);
  constexpr COLOR DarkBlue = toRGB("#00008B", 1.0f);
  constexpr COLOR Navy = toRGB("#000080", 1.0f);
  constexpr COLOR MidnightBlue = toRGB("#191970", 1.0f);

  // Brown Color
  constexpr COLOR Cornsilk = toRGB("#FFF8DC", 1.0f);
  constexpr COLOR BlanchedAlmond = toRGB("#FFEBCD", 1.0f);
  constexpr COLOR Bisque = toRGB("#FFE4C4", 1.0f);
  constexpr COLOR NavajoWhite = toRGB("#FFDEAD", 1.0f);
  constexpr COLOR Wheat = toRGB("#F5DEB3", 1.0f);
  constexpr COLOR BurlyWood = toRGB("#DEB887", 1.0f);
  constexpr COLOR Tan = toRGB("#D2B48C", 1.0f);
  constexpr COLOR RosyBrown = toRGB("#BC8F8F", 1.0f);
  constexpr COLOR SandyBrown = toRGB("#F4A460", 1.0f);
  constexpr COLOR Goldenrod = toRGB("#DAA520", 1.0f);
  constexpr COLOR DarkGoldenrod = toRGB("#B8860B", 1.0f);
  constexpr COLOR Peru = toRGB("#CD853F", 1.0f);
  constexpr COLOR Chocolate = toRGB("#D2691E", 1.0f);
  constexpr COLOR SaddleBrown = toRGB("#8B4513", 1.0f);
  constexpr COLOR Sienna = toRGB("#A0522D", 1.0f);
  constexpr COLOR Brown = toRGB("#A52A2A", 1.0f);
  constexpr COLOR Maroon = toRGB("#800000", 1.0f);

  // White Color
  constexpr COLOR White = toRGB("#FFFFFF", 1.0f);
  constexpr COLOR Snow = toRGB("#FFFAFA", 1.0f);
  constexpr COLOR HoneyDew = toRGB("#F0FFF0", 1.0f);
  constexpr COLOR MintCream = toRGB("#F5FFFA", 1.0f);
  constexpr COLOR Azure = toRGB("#F0FFFF", 1.0f);
  constexpr COLOR AliceBlue = toRGB("#F0F8FF", 1.0f);
  constexpr COLOR GhostWhite = toRGB("#F8F8FF", 1.0f);
  constexpr COLOR WhiteSmoke = toRGB("#F5F5F5", 1.0f);
  constexpr COLOR SeaShell = toRGB("#FFF5EE", 1.0f);
  constexpr COLOR Beige = toRGB("#F5F5DC", 1.0f);
  constexpr COLOR OldLace = toRGB("#FDF5E6", 1.0f);
  constexpr COLOR FloralWhite = toRGB("#FFFAF0", 1.0f);
  constexpr COLOR Ivory = toRGB("#FFFFF0", 1.0f);
  constexpr COLOR AntiqueWhite = toRGB("#FAEBD7", 1.0f);
  constexpr COLOR Linen = toRGB("#FAF0E6", 1.0f);
  constexpr COLOR LavenderBlush = toRGB("#FFF0F5", 1.0f);
  constexpr COLOR MistyRose = toRGB("#FFE4E1", 1.0f);

  // Gray Color
  constexpr COLOR Gainsboro = toRGB("#DCDCDC", 1.0f);
  constexpr COLOR LightGray = toRGB("#D3D3D3", 1.0f);
  constexpr COLOR Silver = toRGB("#C0C0C0", 1.0f);
  constexpr COLOR DarkGray = toRGB("#A9A9A9", 1.0f);
  constexpr COLOR Gray = toRGB("#808080", 1.0f);
  constexpr COLOR DimGray = toRGB("#696969", 1.0f);
  constexpr COLOR LightSlateGray = toRGB("#778899", 1.0f);
  constexpr COLOR SlateGray = toRGB("#708090", 1.0f);
  constexpr COLOR DarkSlateGray = toRGB("#2F4F4F", 1.0f);
  constexpr COLOR Black = toRGB("#000000", 1.0f);
}