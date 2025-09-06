module;

export module game.ui.component.input_hint;

import std;
import game.context;
import game.input;
import game.types;
import graphic.utils.types;
import graphic.utils.font;

constexpr float box_margin = 24;
constexpr float box_padding = 8;

constexpr float row_margin_top = 18;
constexpr float row_padding_left = 24;
constexpr float row_gap = 8;

constexpr float key_size = 30;
constexpr float keys_left_padding = 8;
constexpr float key_gap = 8;

export using InputKey = std::variant<KeyCode, XButtonCode>;

std::unordered_map<KeyCode, UV> keyboard_key_to_uv = {
  {KeyCode::KK_SPACE, UV{{752, 32}, {32, 16}}},
  {KeyCode::KK_ENTER, UV{{784, 16}, {32, 16}}},
  {KeyCode::KK_W, UV{{656, 64}, {16, 16}}},
  {KeyCode::KK_A, UV{{560, 32}, {16, 16}}},
  {KeyCode::KK_S, UV{{592, 64}, {16, 16}}},
  {KeyCode::KK_D, UV{{608, 32}, {16, 16}}},
  {KeyCode::KK_UP, UV{{560, 0}, {16, 16}}},
  {KeyCode::KK_DOWN, UV{{576, 0}, {16, 16}}},
  {KeyCode::KK_LEFT, UV{{592, 0}, {16, 16}}},
  {KeyCode::KK_RIGHT, UV{{608, 0}, {16, 16}}},
};

std::unordered_map<KeyCode, UV> keyboard_key_on_press_to_uv = {
  {KeyCode::KK_SPACE, UV{{752, 96}, {32, 16}}},
  {KeyCode::KK_ENTER, UV{{784, 80}, {32, 16}}},
  {KeyCode::KK_W, UV{{656, 176}, {16, 16}}},
  {KeyCode::KK_A, UV{{560, 144}, {16, 16}}},
  {KeyCode::KK_S, UV{{592, 176}, {16, 16}}},
  {KeyCode::KK_D, UV{{608, 144}, {16, 16}}},
  {KeyCode::KK_UP, UV{{560, 112}, {16, 16}}},
  {KeyCode::KK_DOWN, UV{{576, 112}, {16, 16}}},
  {KeyCode::KK_LEFT, UV{{592, 112}, {16, 16}}},
  {KeyCode::KK_RIGHT, UV{{608, 112}, {16, 16}}},
};

std::unordered_map<XButtonCode, UV> xbutton_key_to_uv = {
  
};

std::unordered_map<XButtonCode, UV> xbutton_key_on_press_to_uv = {
  
};

export struct InputHint {
  std::wstring label;
  std::vector<InputKey> keys;
};

export struct InputHintProps {
  float opacity;
  std::vector<InputHint> input_hints;

  bool left_aligned = false;
  bool top_aligned = false;
};

struct DisplayInputHint {
  std::wstring label;
  std::vector<InputKey> keys;
  std::vector<UV> uvs;
  std::vector<UV> on_press_uvs;
  std::vector<bool> is_pressed;
};

export class InputHintComponent {
private:
  std::wstring font_key_;
  Font* default_font_;
  FixedPoolIndexType ui_texture_id_;

  Vector2 position_;
  float opacity_;
  bool left_aligned_ = false;
  bool top_aligned_ = false;
  std::vector<DisplayInputHint> input_hints;

public:
  InputHintComponent(GameContext* ctx, const InputHintProps& props);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx);

  void SetOpacity(float opacity) { opacity_ = opacity; }
};
