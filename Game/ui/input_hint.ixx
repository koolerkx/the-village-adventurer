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

constexpr float font_size = 18.0f;

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
  {KeyCode::KK_Q, UV{{560, 64}, {16, 16}}},
  {KeyCode::KK_E, UV{{624, 32}, {16, 16}}},
  {KeyCode::KK_I, UV{{560, 48}, {16, 16}}},
  {KeyCode::KK_R, UV{{576, 64}, {16, 16}}},
  {KeyCode::KK_ESCAPE, UV{{720, 0}, {32, 16}}},
  {KeyCode::KK_F11, UV{{656, 16}, {16, 16}}},
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
  {KeyCode::KK_Q, UV{{560, 176}, {16, 16}}},
  {KeyCode::KK_E, UV{{624, 144}, {16, 16}}},
  {KeyCode::KK_I, UV{{560, 160}, {16, 16}}},
  {KeyCode::KK_R, UV{{576, 176}, {16, 16}}},
  {KeyCode::KK_ESCAPE, UV{{720, 64}, {32, 16}}},
  {KeyCode::KK_F11, UV{{656, 128}, {16, 16}}},
};

std::unordered_map<XButtonCode, UV> xbutton_key_to_uv = {
  {XButtonCode::X, UV{{528, 256}, {16, 16}}},
  {XButtonCode::A, UV{{528, 272}, {16, 16}}},
  {XButtonCode::B, UV{{528, 304}, {16, 16}}},
  {XButtonCode::Y, UV{{528, 288}, {16, 16}}},
  {XButtonCode::Start, UV{{528, 336}, {16, 16}}},
  {XButtonCode::Back, UV{{528, 320}, {16, 16}}},
  {XButtonCode::DPadLeft, UV{{640, 320}, {16, 16}}},
  {XButtonCode::DPadRight, UV{{656, 320}, {16, 16}}},
  {XButtonCode::DPadUp, UV{{640, 304}, {16, 16}}},
  {XButtonCode::DPadDown, UV{{656, 304}, {16, 16}}},
  {XButtonCode::LeftThumb, UV{{728, 296}, {16, 16}}},
  {XButtonCode::LB, UV{{528, 464}, {16, 16}}},
  {XButtonCode::RB, UV{{528, 480}, {16, 16}}},
};

std::unordered_map<XButtonCode, UV> xbutton_key_on_press_to_uv = {
  {XButtonCode::X, UV{{592, 256}, {16, 16}}},
  {XButtonCode::A, UV{{592, 272}, {16, 16}}},
  {XButtonCode::B, UV{{592, 304}, {16, 16}}},
  {XButtonCode::Y, UV{{592, 288}, {16, 16}}},
  {XButtonCode::Start, UV{{592, 336}, {16, 16}}},
  {XButtonCode::Back, UV{{592, 320}, {16, 16}}},
  {XButtonCode::DPadLeft, UV{{672, 320}, {16, 16}}},
  {XButtonCode::DPadRight, UV{{688, 320}, {16, 16}}},
  {XButtonCode::DPadUp, UV{{672, 304}, {16, 16}}},
  {XButtonCode::DPadDown, UV{{688, 304}, {16, 16}}},
  {XButtonCode::LeftThumb, UV{{728, 296}, {16, 16}}},
  {XButtonCode::LB, UV{{592, 464}, {16, 16}}},
  {XButtonCode::RB, UV{{592, 480}, {16, 16}}},
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

  float box_width_ = 0;

public:
  InputHintComponent(GameContext* ctx, const InputHintProps& props);
  void OnUpdate(GameContext* ctx, float delta_time);
  void OnFixedUpdate(GameContext* ctx, float delta_time);
  void OnRender(GameContext* ctx);

  void SetOpacity(float opacity) { opacity_ = opacity; }
};
