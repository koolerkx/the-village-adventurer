module;
#include "stdint.h"

export module game.result_scene;

import std;
import game.scene;
import game.ui.result_ui;
import game.utils.throttle;
import game.types;

export struct ResultSceneProps {
  int monster_killed;
  float elapsed_seconds;
  int level;
  bool is_default_x_input;
};

enum class SelectedOption: uint8_t {
  RESTART,
  BACK_TO_TITLE
};

constexpr int score_multiplier_monster = 100;
constexpr int score_multiplier_level = 200;
constexpr int score_multiplier_time = 10;
const std::string default_ranking_file = "save.dat";

export class ResultScene : public IScene {
private:
  std::unique_ptr<ResultUI> result_ui_{nullptr};

  const int options_count = 2;
  uint8_t selected_option_ = 0;

  Throttle input_throttle_{0.3f};
  Throttle enter_throttle_{0.2f};
  bool is_allow_control_ = true;

  int monster_killed_ = 0;
  int minutes_ = 0;
  int seconds_ = 0;
  int level_ = 0;

  int score_ = 0;

  std::vector<RankingItem> ranking_;

  static inline std::int64_t NowEpochMillis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  }

  // sort ranking descending
  static inline void SortRanking(std::vector<RankingItem>& v) {
    std::ranges::sort(v, [](const RankingItem& a, const RankingItem& b) {
      if (a.score != b.score) return a.score > b.score;
      return a.timestamp_ms > b.timestamp_ms;
    });
  }

  static std::vector<RankingItem> LoadRanking(const std::string& filepath = default_ranking_file);
  static void SaveRanking(const std::vector<RankingItem>& items, const std::string& filepath = default_ranking_file);
  static void CreateEmptyRankingFile(const std::string& filepath = default_ranking_file);

  bool is_x_input_;
public:
  ResultScene(ResultSceneProps props);
  void OnEnter(GameContext* ctx) override;
  void OnUpdate(GameContext* ctx, float delta_time) override;
  void OnFixedUpdate(GameContext* ctx, float delta_time) override;
  void OnRender(GameContext* ctx) override;
  void OnExit(GameContext* ctx) override;
};
