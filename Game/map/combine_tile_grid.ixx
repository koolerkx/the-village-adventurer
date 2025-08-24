module;

export module game.map.combine_grid;

import std;
import game.collision.collider;

struct IRect {
  int x, y, w, h;
};

struct Key {
  int x;
  int w;
  bool operator==(const Key& other) const { return x == other.x && w == other.w; }
};
struct KeyHash {
  std::size_t operator()(const Key& k) const noexcept {
    return (std::hash<int>{}(k.x) << 1) ^ std::hash<int>{}(k.w);
  }
};

export std::vector<RectCollider> MergeTilesToRects(const std::vector<RectCollider>& tiles, float tileW, float tileH)
{
  if (tiles.empty()) return {};

  // 0) Establish origin (minX,minY) for stable indexing
  float minX = tiles[0].x, minY = tiles[0].y;
  for (const auto& t : tiles) {
    minX = std::min(minX, t.x);
    minY = std::min(minY, t.y);
  }

  // 1) Convert each tile's top-left to integer grid (gx,gy) via *nearest integer*.
  //    Using round avoids off-by-one gaps that keep runs at length 1.
  struct GCell { int gx, gy; };
  std::vector<GCell> cells; cells.reserve(tiles.size());
  int maxGX = 0, maxGY = 0;
  for (const auto& t : tiles) {
    // Optional sanity: validate that tile sizes match the provided tileW/H (within epsilon).
    // If they don't, you could either normalize by tileW/H or compute coverage over multiple cells.
    // Here we assume one tile == one grid cell.
    int gx = static_cast<int>(std::llround((t.x - minX) / tileW));
    int gy = static_cast<int>(std::llround((t.y - minY) / tileH));
    maxGX = std::max(maxGX, gx);
    maxGY = std::max(maxGY, gy);
    cells.push_back({gx, gy});
  }

  // 2) Build solid grid sized by observed max indices (robust to rounding jitter)
  const int W = maxGX + 1;
  const int H = maxGY + 1;
  if (W <= 0 || H <= 0) return {};

  std::vector<bool> solid(static_cast<size_t>(W) * H, false);
  for (const auto& c : cells) {
    if (c.gx >= 0 && c.gx < W && c.gy >= 0 && c.gy < H) {
      solid[c.gy * W + c.gx] = true;
    }
  }

  // 3) RLE per row to produce 1-row rectangles
  std::vector<std::vector<IRect>> rowRuns(H);
  for (int y = 0; y < H; ++y) {
    int x = 0;
    while (x < W) {
      while (x < W && !solid[y * W + x]) ++x;
      if (x >= W) break;
      const int start = x;
      while (x < W && solid[y * W + x]) ++x;
      const int len = x - start;
      rowRuns[y].push_back(IRect{start, y, len, 1});
    }
  }

  // 4) Vertical merge of identical (x,w) runs across consecutive rows
  std::vector<IRect> merged; merged.reserve(W * H / 4 + 8);
  std::unordered_map<Key, int, KeyHash> activeAbove;
  for (int y = 0; y < H; ++y) {
    std::unordered_map<Key, int, KeyHash> nextActive;
    for (const IRect& run : rowRuns[y]) {
      Key k{run.x, run.w};
      auto it = activeAbove.find(k);
      if (it != activeAbove.end()) {
        const int idx = it->second;
        merged[idx].h += 1; // extend height
        nextActive.emplace(k, idx);
      } else {
        const int idx = static_cast<int>(merged.size());
        merged.push_back(run);
        nextActive.emplace(k, idx);
      }
    }
    activeAbove.swap(nextActive);
  }

  // 5) Convert back to world-space rects
  std::vector<RectCollider> out; out.reserve(merged.size());
  for (const IRect& r : merged) {
    RectCollider R;
    R.x = minX + r.x * tileW;
    R.y = minY + r.y * tileH;
    R.width = r.w * tileW;   // r.w and r.h are in cell counts; multiply by tile size
    R.height = r.h * tileH;
    out.push_back(R);
  }
  return out;
}
