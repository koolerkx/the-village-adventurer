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
  constexpr COLOR indianRed = toRGB("#CD5C5C", 1.0f);
  constexpr COLOR lightCoral = toRGB("#F08080", 1.0f);
  constexpr COLOR salmon = toRGB("#FA8072", 1.0f);
  constexpr COLOR darkSalmon = toRGB("#E9967A", 1.0f);
  constexpr COLOR lightSalmon = toRGB("#FFA07A", 1.0f);
  constexpr COLOR crimson = toRGB("#DC143C", 1.0f);
  constexpr COLOR red = toRGB("#FF0000", 1.0f);
  constexpr COLOR fireBrick = toRGB("#B22222", 1.0f);
  constexpr COLOR darkRed = toRGB("#8B0000", 1.0f);

  // Pink Color
  constexpr COLOR pink = toRGB("#FFC0CB", 1.0f);
  constexpr COLOR lightPink = toRGB("#FFB6C1", 1.0f);
  constexpr COLOR hotPink = toRGB("#FF69B4", 1.0f);
  constexpr COLOR deepPink = toRGB("#FF1493", 1.0f);
  constexpr COLOR mediumVioletRed = toRGB("#C71585", 1.0f);
  constexpr COLOR paleVioletRed = toRGB("#DB7093", 1.0f);

  // Orange Color
  constexpr COLOR lightSalmonOrange = toRGB("#FFA07A", 1.0f); // Note: LightSalmon appears in both Red and Orange
  constexpr COLOR coral = toRGB("#FF7F50", 1.0f);
  constexpr COLOR tomato = toRGB("#FF6347", 1.0f);
  constexpr COLOR orangeRed = toRGB("#FF4500", 1.0f);
  constexpr COLOR darkOrange = toRGB("#FF8C00", 1.0f);
  constexpr COLOR orange = toRGB("#FFA500", 1.0f);

  // Yellow Color
  constexpr COLOR gold = toRGB("#FFD700", 1.0f);
  constexpr COLOR yellow = toRGB("#FFFF00", 1.0f);
  constexpr COLOR lightYellow = toRGB("#FFFFE0", 1.0f);
  constexpr COLOR lemonChiffon = toRGB("#FFFACD", 1.0f);
  constexpr COLOR lightGoldenrodYellow = toRGB("#FAFAD2", 1.0f);
  constexpr COLOR papayaWhip = toRGB("#FFEFD5", 1.0f);
  constexpr COLOR moccasin = toRGB("#FFE4B5", 1.0f);
  constexpr COLOR peachPuff = toRGB("#FFDAB9", 1.0f);
  constexpr COLOR paleGoldenrod = toRGB("#EEE8AA", 1.0f);
  constexpr COLOR khaki = toRGB("#F0E68C", 1.0f);
  constexpr COLOR darkKhaki = toRGB("#BDB76B", 1.0f);

  // Purple Color
  constexpr COLOR lavender = toRGB("#E6E6FA", 1.0f);
  constexpr COLOR thistle = toRGB("#D8BFD8", 1.0f);
  constexpr COLOR plum = toRGB("#DDA0DD", 1.0f);
  constexpr COLOR violet = toRGB("#EE82EE", 1.0f);
  constexpr COLOR orchid = toRGB("#DA70D6", 1.0f);
  constexpr COLOR fuchsia = toRGB("#FF00FF", 1.0f);
  constexpr COLOR magenta = toRGB("#FF00FF", 1.0f);
  constexpr COLOR mediumOrchid = toRGB("#BA55D3", 1.0f);
  constexpr COLOR mediumPurple = toRGB("#9370DB", 1.0f);
  constexpr COLOR rebeccaPurple = toRGB("#663399", 1.0f);
  constexpr COLOR blueViolet = toRGB("#8A2BE2", 1.0f);
  constexpr COLOR darkViolet = toRGB("#9400D3", 1.0f);
  constexpr COLOR darkOrchid = toRGB("#9932CC", 1.0f);
  constexpr COLOR darkMagenta = toRGB("#8B008B", 1.0f);
  constexpr COLOR purple = toRGB("#800080", 1.0f);
  constexpr COLOR indigo = toRGB("#4B0082", 1.0f);
  constexpr COLOR slateBlue = toRGB("#6A5ACD", 1.0f);
  constexpr COLOR darkSlateBlue = toRGB("#483D8B", 1.0f);
  constexpr COLOR mediumSlateBlue = toRGB("#7B68EE", 1.0f);

  // Green Color
  constexpr COLOR greenYellow = toRGB("#ADFF2F", 1.0f);
  constexpr COLOR chartreuse = toRGB("#7FFF00", 1.0f);
  constexpr COLOR lawnGreen = toRGB("#7CFC00", 1.0f);
  constexpr COLOR lime = toRGB("#00FF00", 1.0f);
  constexpr COLOR limeGreen = toRGB("#32CD32", 1.0f);
  constexpr COLOR paleGreen = toRGB("#98FB98", 1.0f);
  constexpr COLOR lightGreen = toRGB("#90EE90", 1.0f);
  constexpr COLOR mediumSpringGreen = toRGB("#00FA9A", 1.0f);
  constexpr COLOR springGreen = toRGB("#00FF7F", 1.0f);
  constexpr COLOR mediumSeaGreen = toRGB("#3CB371", 1.0f);
  constexpr COLOR seaGreen = toRGB("#2E8B57", 1.0f);
  constexpr COLOR forestGreen = toRGB("#228B22", 1.0f);
  constexpr COLOR green = toRGB("#008000", 1.0f);
  constexpr COLOR darkGreen = toRGB("#006400", 1.0f);
  constexpr COLOR yellowGreen = toRGB("#9ACD32", 1.0f);
  constexpr COLOR oliveDrab = toRGB("#6B8E23", 1.0f);
  constexpr COLOR olive = toRGB("#808000", 1.0f);
  constexpr COLOR darkOliveGreen = toRGB("#556B2F", 1.0f);
  constexpr COLOR mediumAquamarine = toRGB("#66CDAA", 1.0f);
  constexpr COLOR darkSeaGreen = toRGB("#8FBC8B", 1.0f);
  constexpr COLOR lightSeaGreen = toRGB("#20B2AA", 1.0f);
  constexpr COLOR darkCyan = toRGB("#008B8B", 1.0f);
  constexpr COLOR teal = toRGB("#008080", 1.0f);

  // Blue Color
  constexpr COLOR aqua = toRGB("#00FFFF", 1.0f);
  constexpr COLOR cyan = toRGB("#00FFFF", 1.0f);
  constexpr COLOR lightCyan = toRGB("#E0FFFF", 1.0f);
  constexpr COLOR paleTurquoise = toRGB("#AFEEEE", 1.0f);
  constexpr COLOR aquamarine = toRGB("#7FFFD4", 1.0f);
  constexpr COLOR turquoise = toRGB("#40E0D0", 1.0f);
  constexpr COLOR mediumTurquoise = toRGB("#48D1CC", 1.0f);
  constexpr COLOR darkTurquoise = toRGB("#00CED1", 1.0f);
  constexpr COLOR cadetBlue = toRGB("#5F9EA0", 1.0f);
  constexpr COLOR steelBlue = toRGB("#4682B4", 1.0f);
  constexpr COLOR lightSteelBlue = toRGB("#B0C4DE", 1.0f);
  constexpr COLOR powderBlue = toRGB("#B0E0E6", 1.0f);
  constexpr COLOR lightBlue = toRGB("#ADD8E6", 1.0f);
  constexpr COLOR skyBlue = toRGB("#87CEEB", 1.0f);
  constexpr COLOR lightSkyBlue = toRGB("#87CEFA", 1.0f);
  constexpr COLOR deepSkyBlue = toRGB("#00BFFF", 1.0f);
  constexpr COLOR dodgerBlue = toRGB("#1E90FF", 1.0f);
  constexpr COLOR cornflowerBlue = toRGB("#6495ED", 1.0f);
  constexpr COLOR mediumSlateBlueBlue = toRGB("#7B68EE", 1.0f); // Note: MediumSlateBlue appears in both Purple and Blue
  constexpr COLOR royalBlue = toRGB("#4169E1", 1.0f);
  constexpr COLOR blue = toRGB("#0000FF", 1.0f);
  constexpr COLOR mediumBlue = toRGB("#0000CD", 1.0f);
  constexpr COLOR darkBlue = toRGB("#00008B", 1.0f);
  constexpr COLOR navy = toRGB("#000080", 1.0f);
  constexpr COLOR midnightBlue = toRGB("#191970", 1.0f);

  // Brown Color
  constexpr COLOR cornsilk = toRGB("#FFF8DC", 1.0f);
  constexpr COLOR blanchedAlmond = toRGB("#FFEBCD", 1.0f);
  constexpr COLOR bisque = toRGB("#FFE4C4", 1.0f);
  constexpr COLOR navajoWhite = toRGB("#FFDEAD", 1.0f);
  constexpr COLOR wheat = toRGB("#F5DEB3", 1.0f);
  constexpr COLOR burlyWood = toRGB("#DEB887", 1.0f);
  constexpr COLOR tan = toRGB("#D2B48C", 1.0f);
  constexpr COLOR rosyBrown = toRGB("#BC8F8F", 1.0f);
  constexpr COLOR sandyBrown = toRGB("#F4A460", 1.0f);
  constexpr COLOR goldenrod = toRGB("#DAA520", 1.0f);
  constexpr COLOR darkGoldenrod = toRGB("#B8860B", 1.0f);
  constexpr COLOR peru = toRGB("#CD853F", 1.0f);
  constexpr COLOR chocolate = toRGB("#D2691E", 1.0f);
  constexpr COLOR saddleBrown = toRGB("#8B4513", 1.0f);
  constexpr COLOR sienna = toRGB("#A0522D", 1.0f);
  constexpr COLOR brown = toRGB("#A52A2A", 1.0f);
  constexpr COLOR maroon = toRGB("#800000", 1.0f);

  // White Color
  constexpr COLOR white = toRGB("#FFFFFF", 1.0f);
  constexpr COLOR snow = toRGB("#FFFAFA", 1.0f);
  constexpr COLOR honeyDew = toRGB("#F0FFF0", 1.0f);
  constexpr COLOR mintCream = toRGB("#F5FFFA", 1.0f);
  constexpr COLOR azure = toRGB("#F0FFFF", 1.0f);
  constexpr COLOR aliceBlue = toRGB("#F0F8FF", 1.0f);
  constexpr COLOR ghostWhite = toRGB("#F8F8FF", 1.0f);
  constexpr COLOR whiteSmoke = toRGB("#F5F5F5", 1.0f);
  constexpr COLOR seaShell = toRGB("#FFF5EE", 1.0f);
  constexpr COLOR beige = toRGB("#F5F5DC", 1.0f);
  constexpr COLOR oldLace = toRGB("#FDF5E6", 1.0f);
  constexpr COLOR floralWhite = toRGB("#FFFAF0", 1.0f);
  constexpr COLOR ivory = toRGB("#FFFFF0", 1.0f);
  constexpr COLOR antiqueWhite = toRGB("#FAEBD7", 1.0f);
  constexpr COLOR linen = toRGB("#FAF0E6", 1.0f);
  constexpr COLOR lavenderBlush = toRGB("#FFF0F5", 1.0f);
  constexpr COLOR mistyRose = toRGB("#FFE4E1", 1.0f);

  // Gray Color
  constexpr COLOR gainsboro = toRGB("#DCDCDC", 1.0f);
  constexpr COLOR lightGray = toRGB("#D3D3D3", 1.0f);
  constexpr COLOR silver = toRGB("#C0C0C0", 1.0f);
  constexpr COLOR darkGray = toRGB("#A9A9A9", 1.0f);
  constexpr COLOR gray = toRGB("#808080", 1.0f);
  constexpr COLOR dimGray = toRGB("#696969", 1.0f);
  constexpr COLOR lightSlateGray = toRGB("#778899", 1.0f);
  constexpr COLOR slateGray = toRGB("#708090", 1.0f);
  constexpr COLOR darkSlateGray = toRGB("#2F4F4F", 1.0f);
  constexpr COLOR black = toRGB("#000000", 1.0f);

  // Material Design 2 Colors: 2014 Material Design color palettes
  // https://mui.com/material-ui/customization/color/
  // https://m2.material.io/design/color
  // Red
  constexpr COLOR red50 = toRGB("#ffebee", 1.0f);
  constexpr COLOR red100 = toRGB("#ffcdd2", 1.0f);
  constexpr COLOR red200 = toRGB("#ef9a9a", 1.0f);
  constexpr COLOR red300 = toRGB("#e57373", 1.0f);
  constexpr COLOR red400 = toRGB("#ef5350", 1.0f);
  constexpr COLOR red500 = toRGB("#f44336", 1.0f);
  constexpr COLOR red600 = toRGB("#e53935", 1.0f);
  constexpr COLOR red700 = toRGB("#d32f2f", 1.0f);
  constexpr COLOR red800 = toRGB("#c62828", 1.0f);
  constexpr COLOR red900 = toRGB("#b71c1c", 1.0f);
  constexpr COLOR redA100 = toRGB("#ff8a80", 1.0f);
  constexpr COLOR redA200 = toRGB("#ff5252", 1.0f);
  constexpr COLOR redA400 = toRGB("#ff1744", 1.0f);
  constexpr COLOR redA700 = toRGB("#d50000", 1.0f);

  // Pink
  constexpr COLOR pink50 = toRGB("#fce4ec", 1.0f);
  constexpr COLOR pink100 = toRGB("#f8bbd0", 1.0f);
  constexpr COLOR pink200 = toRGB("#f48fb1", 1.0f);
  constexpr COLOR pink300 = toRGB("#f06292", 1.0f);
  constexpr COLOR pink400 = toRGB("#ec407a", 1.0f);
  constexpr COLOR pink500 = toRGB("#e91e63", 1.0f);
  constexpr COLOR pink600 = toRGB("#d81b60", 1.0f);
  constexpr COLOR pink700 = toRGB("#c2185b", 1.0f);
  constexpr COLOR pink800 = toRGB("#ad1457", 1.0f);
  constexpr COLOR pink900 = toRGB("#880e4f", 1.0f);
  constexpr COLOR pinkA100 = toRGB("#ff80ab", 1.0f);
  constexpr COLOR pinkA200 = toRGB("#ff4081", 1.0f);
  constexpr COLOR pinkA400 = toRGB("#f50057", 1.0f);
  constexpr COLOR pinkA700 = toRGB("#c51162", 1.0f);

  // Purple
  constexpr COLOR purple50 = toRGB("#f3e5f5", 1.0f);
  constexpr COLOR purple100 = toRGB("#e1bee7", 1.0f);
  constexpr COLOR purple200 = toRGB("#ce93d8", 1.0f);
  constexpr COLOR purple300 = toRGB("#ba68c8", 1.0f);
  constexpr COLOR purple400 = toRGB("#ab47bc", 1.0f);
  constexpr COLOR purple500 = toRGB("#9c27b0", 1.0f);
  constexpr COLOR purple600 = toRGB("#8e24aa", 1.0f);
  constexpr COLOR purple700 = toRGB("#7b1fa2", 1.0f);
  constexpr COLOR purple800 = toRGB("#6a1b9a", 1.0f);
  constexpr COLOR purple900 = toRGB("#4a148c", 1.0f);
  constexpr COLOR purpleA100 = toRGB("#ea80fc", 1.0f);
  constexpr COLOR purpleA200 = toRGB("#e040fb", 1.0f);
  constexpr COLOR purpleA400 = toRGB("#d500f9", 1.0f);
  constexpr COLOR purpleA700 = toRGB("#aa00ff", 1.0f);

  // Deep Purple
  constexpr COLOR deepPurple50 = toRGB("#ede7f6", 1.0f);
  constexpr COLOR deepPurple100 = toRGB("#d1c4e9", 1.0f);
  constexpr COLOR deepPurple200 = toRGB("#b39ddb", 1.0f);
  constexpr COLOR deepPurple300 = toRGB("#9575cd", 1.0f);
  constexpr COLOR deepPurple400 = toRGB("#7e57c2", 1.0f);
  constexpr COLOR deepPurple500 = toRGB("#673ab7", 1.0f);
  constexpr COLOR deepPurple600 = toRGB("#5e35b1", 1.0f);
  constexpr COLOR deepPurple700 = toRGB("#512da8", 1.0f);
  constexpr COLOR deepPurple800 = toRGB("#4527a0", 1.0f);
  constexpr COLOR deepPurple900 = toRGB("#311b92", 1.0f);
  constexpr COLOR deepPurpleA100 = toRGB("#b388ff", 1.0f);
  constexpr COLOR deepPurpleA200 = toRGB("#7c4dff", 1.0f);
  constexpr COLOR deepPurpleA400 = toRGB("#651fff", 1.0f);
  constexpr COLOR deepPurpleA700 = toRGB("#6200ea", 1.0f);

  // Indigo
  constexpr COLOR indigo50 = toRGB("#e8eaf6", 1.0f);
  constexpr COLOR indigo100 = toRGB("#c5cae9", 1.0f);
  constexpr COLOR indigo200 = toRGB("#9fa8da", 1.0f);
  constexpr COLOR indigo300 = toRGB("#7986cb", 1.0f);
  constexpr COLOR indigo400 = toRGB("#5c6bc0", 1.0f);
  constexpr COLOR indigo500 = toRGB("#3f51b5", 1.0f);
  constexpr COLOR indigo600 = toRGB("#3949ab", 1.0f);
  constexpr COLOR indigo700 = toRGB("#303f9f", 1.0f);
  constexpr COLOR indigo800 = toRGB("#283593", 1.0f);
  constexpr COLOR indigo900 = toRGB("#1a237e", 1.0f);
  constexpr COLOR indigoA100 = toRGB("#8c9eff", 1.0f);
  constexpr COLOR indigoA200 = toRGB("#536dfe", 1.0f);
  constexpr COLOR indigoA400 = toRGB("#3d5afe", 1.0f);
  constexpr COLOR indigoA700 = toRGB("#304ffe", 1.0f);

  // Blue
  constexpr COLOR blue50 = toRGB("#e3f2fd", 1.0f);
  constexpr COLOR blue100 = toRGB("#bbdefb", 1.0f);
  constexpr COLOR blue200 = toRGB("#90caf9", 1.0f);
  constexpr COLOR blue300 = toRGB("#64b5f6", 1.0f);
  constexpr COLOR blue400 = toRGB("#42a5f5", 1.0f);
  constexpr COLOR blue500 = toRGB("#2196f3", 1.0f);
  constexpr COLOR blue600 = toRGB("#1e88e5", 1.0f);
  constexpr COLOR blue700 = toRGB("#1976d2", 1.0f);
  constexpr COLOR blue800 = toRGB("#1565c0", 1.0f);
  constexpr COLOR blue900 = toRGB("#0d47a1", 1.0f);
  constexpr COLOR blueA100 = toRGB("#82b1ff", 1.0f);
  constexpr COLOR blueA200 = toRGB("#448aff", 1.0f);
  constexpr COLOR blueA400 = toRGB("#2979ff", 1.0f);
  constexpr COLOR blueA700 = toRGB("#2962ff", 1.0f);

  // Light Blue
  constexpr COLOR lightBlue50 = toRGB("#e1f5fe", 1.0f);
  constexpr COLOR lightBlue100 = toRGB("#b3e5fc", 1.0f);
  constexpr COLOR lightBlue200 = toRGB("#81d4fa", 1.0f);
  constexpr COLOR lightBlue300 = toRGB("#4fc3f7", 1.0f);
  constexpr COLOR lightBlue400 = toRGB("#29b6f6", 1.0f);
  constexpr COLOR lightBlue500 = toRGB("#03a9f4", 1.0f);
  constexpr COLOR lightBlue600 = toRGB("#039be5", 1.0f);
  constexpr COLOR lightBlue700 = toRGB("#0288d1", 1.0f);
  constexpr COLOR lightBlue800 = toRGB("#0277bd", 1.0f);
  constexpr COLOR lightBlue900 = toRGB("#01579b", 1.0f);
  constexpr COLOR lightBlueA100 = toRGB("#80d8ff", 1.0f);
  constexpr COLOR lightBlueA200 = toRGB("#40c4ff", 1.0f);
  constexpr COLOR lightBlueA400 = toRGB("#00b0ff", 1.0f);
  constexpr COLOR lightBlueA700 = toRGB("#0091ea", 1.0f);

  // Cyan
  constexpr COLOR cyan50 = toRGB("#e0f7fa", 1.0f);
  constexpr COLOR cyan100 = toRGB("#b2ebf2", 1.0f);
  constexpr COLOR cyan200 = toRGB("#80deea", 1.0f);
  constexpr COLOR cyan300 = toRGB("#4dd0e1", 1.0f);
  constexpr COLOR cyan400 = toRGB("#26c6da", 1.0f);
  constexpr COLOR cyan500 = toRGB("#00bcd4", 1.0f);
  constexpr COLOR cyan600 = toRGB("#00acc1", 1.0f);
  constexpr COLOR cyan700 = toRGB("#0097a7", 1.0f);
  constexpr COLOR cyan800 = toRGB("#00838f", 1.0f);
  constexpr COLOR cyan900 = toRGB("#006064", 1.0f);
  constexpr COLOR cyanA100 = toRGB("#84ffff", 1.0f);
  constexpr COLOR cyanA200 = toRGB("#18ffff", 1.0f);
  constexpr COLOR cyanA400 = toRGB("#00e5ff", 1.0f);
  constexpr COLOR cyanA700 = toRGB("#00b8d4", 1.0f);

  // Teal
  constexpr COLOR teal50 = toRGB("#e0f2f1", 1.0f);
  constexpr COLOR teal100 = toRGB("#b2dfdb", 1.0f);
  constexpr COLOR teal200 = toRGB("#80cbc4", 1.0f);
  constexpr COLOR teal300 = toRGB("#4db6ac", 1.0f);
  constexpr COLOR teal400 = toRGB("#26a69a", 1.0f);
  constexpr COLOR teal500 = toRGB("#009688", 1.0f);
  constexpr COLOR teal600 = toRGB("#00897b", 1.0f);
  constexpr COLOR teal700 = toRGB("#00796b", 1.0f);
  constexpr COLOR teal800 = toRGB("#00695c", 1.0f);
  constexpr COLOR teal900 = toRGB("#004d40", 1.0f);
  constexpr COLOR tealA100 = toRGB("#a7ffeb", 1.0f);
  constexpr COLOR tealA200 = toRGB("#64ffda", 1.0f);
  constexpr COLOR tealA400 = toRGB("#1de9b6", 1.0f);
  constexpr COLOR tealA700 = toRGB("#00bfa5", 1.0f);

  // Green
  constexpr COLOR green50 = toRGB("#e8f5e9", 1.0f);
  constexpr COLOR green100 = toRGB("#c8e6c9", 1.0f);
  constexpr COLOR green200 = toRGB("#a5d6a7", 1.0f);
  constexpr COLOR green300 = toRGB("#81c784", 1.0f);
  constexpr COLOR green400 = toRGB("#66bb6a", 1.0f);
  constexpr COLOR green500 = toRGB("#4caf50", 1.0f);
  constexpr COLOR green600 = toRGB("#43a047", 1.0f);
  constexpr COLOR green700 = toRGB("#388e3c", 1.0f);
  constexpr COLOR green800 = toRGB("#2e7d32", 1.0f);
  constexpr COLOR green900 = toRGB("#1b5e20", 1.0f);
  constexpr COLOR greenA100 = toRGB("#b9f6ca", 1.0f);
  constexpr COLOR greenA200 = toRGB("#69f0ae", 1.0f);
  constexpr COLOR greenA400 = toRGB("#00e676", 1.0f);
  constexpr COLOR greenA700 = toRGB("#00c853", 1.0f);

  // Light Green
  constexpr COLOR lightGreen50 = toRGB("#f1f8e9", 1.0f);
  constexpr COLOR lightGreen100 = toRGB("#dcedc8", 1.0f);
  constexpr COLOR lightGreen200 = toRGB("#c5e1a5", 1.0f);
  constexpr COLOR lightGreen300 = toRGB("#aed581", 1.0f);
  constexpr COLOR lightGreen400 = toRGB("#9ccc65", 1.0f);
  constexpr COLOR lightGreen500 = toRGB("#8bc34a", 1.0f);
  constexpr COLOR lightGreen600 = toRGB("#7cb342", 1.0f);
  constexpr COLOR lightGreen700 = toRGB("#689f38", 1.0f);
  constexpr COLOR lightGreen800 = toRGB("#558b2f", 1.0f);
  constexpr COLOR lightGreen900 = toRGB("#33691e", 1.0f);
  constexpr COLOR lightGreenA100 = toRGB("#ccff90", 1.0f);
  constexpr COLOR lightGreenA200 = toRGB("#b2ff59", 1.0f);
  constexpr COLOR lightGreenA400 = toRGB("#76ff03", 1.0f);
  constexpr COLOR lightGreenA700 = toRGB("#64dd17", 1.0f);

  // Lime
  constexpr COLOR lime50 = toRGB("#f9fbe7", 1.0f);
  constexpr COLOR lime100 = toRGB("#f0f4c3", 1.0f);
  constexpr COLOR lime200 = toRGB("#e6ee9c", 1.0f);
  constexpr COLOR lime300 = toRGB("#dce775", 1.0f);
  constexpr COLOR lime400 = toRGB("#d4e157", 1.0f);
  constexpr COLOR lime500 = toRGB("#cddc39", 1.0f);
  constexpr COLOR lime600 = toRGB("#c0ca33", 1.0f);
  constexpr COLOR lime700 = toRGB("#afb42b", 1.0f);
  constexpr COLOR lime800 = toRGB("#9e9d24", 1.0f);
  constexpr COLOR lime900 = toRGB("#827717", 1.0f);
  constexpr COLOR limeA100 = toRGB("#f4ff81", 1.0f);
  constexpr COLOR limeA200 = toRGB("#eeff41", 1.0f);
  constexpr COLOR limeA400 = toRGB("#c6ff00", 1.0f);
  constexpr COLOR limeA700 = toRGB("#aeea00", 1.0f);

  // Yellow
  constexpr COLOR yellow50 = toRGB("#fffde7", 1.0f);
  constexpr COLOR yellow100 = toRGB("#fff9c4", 1.0f);
  constexpr COLOR yellow200 = toRGB("#fff59d", 1.0f);
  constexpr COLOR yellow300 = toRGB("#fff176", 1.0f);
  constexpr COLOR yellow400 = toRGB("#ffee58", 1.0f);
  constexpr COLOR yellow500 = toRGB("#ffeb3b", 1.0f);
  constexpr COLOR yellow600 = toRGB("#fdd835", 1.0f);
  constexpr COLOR yellow700 = toRGB("#fbc02d", 1.0f);
  constexpr COLOR yellow800 = toRGB("#f9a825", 1.0f);
  constexpr COLOR yellow900 = toRGB("#f57f17", 1.0f);
  constexpr COLOR yellowA100 = toRGB("#ffff8d", 1.0f);
  constexpr COLOR yellowA200 = toRGB("#ffff00", 1.0f);
  constexpr COLOR yellowA400 = toRGB("#ffea00", 1.0f);
  constexpr COLOR yellowA700 = toRGB("#ffd600", 1.0f);

  // Amber
  constexpr COLOR amber50 = toRGB("#fff8e1", 1.0f);
  constexpr COLOR amber100 = toRGB("#ffecb3", 1.0f);
  constexpr COLOR amber200 = toRGB("#ffe082", 1.0f);
  constexpr COLOR amber300 = toRGB("#ffd54f", 1.0f);
  constexpr COLOR amber400 = toRGB("#ffca28", 1.0f);
  constexpr COLOR amber500 = toRGB("#ffc107", 1.0f);
  constexpr COLOR amber600 = toRGB("#ffb300", 1.0f);
  constexpr COLOR amber700 = toRGB("#ffa000", 1.0f);
  constexpr COLOR amber800 = toRGB("#ff8f00", 1.0f);
  constexpr COLOR amber900 = toRGB("#ff6f00", 1.0f);
  constexpr COLOR amberA100 = toRGB("#ffe57f", 1.0f);
  constexpr COLOR amberA200 = toRGB("#ffd740", 1.0f);
  constexpr COLOR amberA400 = toRGB("#ffc400", 1.0f);
  constexpr COLOR amberA700 = toRGB("#ffab00", 1.0f);

  // Orange
  constexpr COLOR orange50 = toRGB("#fff3e0", 1.0f);
  constexpr COLOR orange100 = toRGB("#ffe0b2", 1.0f);
  constexpr COLOR orange200 = toRGB("#ffcc80", 1.0f);
  constexpr COLOR orange300 = toRGB("#ffb74d", 1.0f);
  constexpr COLOR orange400 = toRGB("#ffa726", 1.0f);
  constexpr COLOR orange500 = toRGB("#ff9800", 1.0f);
  constexpr COLOR orange600 = toRGB("#fb8c00", 1.0f);
  constexpr COLOR orange700 = toRGB("#f57c00", 1.0f);
  constexpr COLOR orange800 = toRGB("#ef6c00", 1.0f);
  constexpr COLOR orange900 = toRGB("#e65100", 1.0f);
  constexpr COLOR orangeA100 = toRGB("#ffd180", 1.0f);
  constexpr COLOR orangeA200 = toRGB("#ffab40", 1.0f);
  constexpr COLOR orangeA400 = toRGB("#ff9100", 1.0f);
  constexpr COLOR orangeA700 = toRGB("#ff6d00", 1.0f);

  // Deep Orange
  constexpr COLOR deepOrange50 = toRGB("#fbe9e7", 1.0f);
  constexpr COLOR deepOrange100 = toRGB("#ffccbc", 1.0f);
  constexpr COLOR deepOrange200 = toRGB("#ffab91", 1.0f);
  constexpr COLOR deepOrange300 = toRGB("#ff8a65", 1.0f);
  constexpr COLOR deepOrange400 = toRGB("#ff7043", 1.0f);
  constexpr COLOR deepOrange500 = toRGB("#ff5722", 1.0f);
  constexpr COLOR deepOrange600 = toRGB("#f4511e", 1.0f);
  constexpr COLOR deepOrange700 = toRGB("#e64a19", 1.0f);
  constexpr COLOR deepOrange800 = toRGB("#d84315", 1.0f);
  constexpr COLOR deepOrange900 = toRGB("#bf360c", 1.0f);
  constexpr COLOR deepOrangeA100 = toRGB("#ff9e80", 1.0f);
  constexpr COLOR deepOrangeA200 = toRGB("#ff6e40", 1.0f);
  constexpr COLOR deepOrangeA400 = toRGB("#ff3d00", 1.0f);
  constexpr COLOR deepOrangeA700 = toRGB("#dd2c00", 1.0f);

  // Brown
  constexpr COLOR brown50 = toRGB("#efebe9", 1.0f);
  constexpr COLOR brown100 = toRGB("#d7ccc8", 1.0f);
  constexpr COLOR brown200 = toRGB("#bcaaa4", 1.0f);
  constexpr COLOR brown300 = toRGB("#a1887f", 1.0f);
  constexpr COLOR brown400 = toRGB("#8d6e63", 1.0f);
  constexpr COLOR brown500 = toRGB("#795548", 1.0f);
  constexpr COLOR brown600 = toRGB("#6d4c41", 1.0f);
  constexpr COLOR brown700 = toRGB("#5d4037", 1.0f);
  constexpr COLOR brown800 = toRGB("#4e342e", 1.0f);
  constexpr COLOR brown900 = toRGB("#3e2723", 1.0f);
  constexpr COLOR brownA100 = toRGB("#d7ccc8", 1.0f);
  constexpr COLOR brownA200 = toRGB("#bcaaa4", 1.0f);
  constexpr COLOR brownA400 = toRGB("#8d6e63", 1.0f);
  constexpr COLOR brownA700 = toRGB("#5d4037", 1.0f);

  // Grey
  constexpr COLOR grey50 = toRGB("#fafafa", 1.0f);
  constexpr COLOR grey100 = toRGB("#f5f5f5", 1.0f);
  constexpr COLOR grey200 = toRGB("#eeeeee", 1.0f);
  constexpr COLOR grey300 = toRGB("#e0e0e0", 1.0f);
  constexpr COLOR grey400 = toRGB("#bdbdbd", 1.0f);
  constexpr COLOR grey500 = toRGB("#9e9e9e", 1.0f);
  constexpr COLOR grey600 = toRGB("#757575", 1.0f);
  constexpr COLOR grey700 = toRGB("#616161", 1.0f);
  constexpr COLOR grey800 = toRGB("#424242", 1.0f);
  constexpr COLOR grey900 = toRGB("#212121", 1.0f);
  constexpr COLOR greyA100 = toRGB("#f5f5f5", 1.0f);
  constexpr COLOR greyA200 = toRGB("#eeeeee", 1.0f);
  constexpr COLOR greyA400 = toRGB("#bdbdbd", 1.0f);
  constexpr COLOR greyA700 = toRGB("#616161", 1.0f);

  // Blue Grey
  constexpr COLOR blueGrey50 = toRGB("#eceff1", 1.0f);
  constexpr COLOR blueGrey100 = toRGB("#cfd8dc", 1.0f);
  constexpr COLOR blueGrey200 = toRGB("#b0bec5", 1.0f);
  constexpr COLOR blueGrey300 = toRGB("#90a4ae", 1.0f);
  constexpr COLOR blueGrey400 = toRGB("#78909c", 1.0f);
  constexpr COLOR blueGrey500 = toRGB("#607d8b", 1.0f);
  constexpr COLOR blueGrey600 = toRGB("#546e7a", 1.0f);
  constexpr COLOR blueGrey700 = toRGB("#455a64", 1.0f);
  constexpr COLOR blueGrey800 = toRGB("#37474f", 1.0f);
  constexpr COLOR blueGrey900 = toRGB("#263238", 1.0f);
  constexpr COLOR blueGreyA100 = toRGB("#cfd8dc", 1.0f);
  constexpr COLOR blueGreyA200 = toRGB("#b0bec5", 1.0f);
  constexpr COLOR blueGreyA400 = toRGB("#78909c", 1.0f);
  constexpr COLOR blueGreyA700 = toRGB("#455a64", 1.0f);


  DirectX::XMFLOAT4 setOpacity(COLOR color, float opacity) {
    return DirectX::XMFLOAT4(color.x, color.y, color.z, opacity);
  }
}