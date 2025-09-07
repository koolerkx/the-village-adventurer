module;
#include "key_logger.h"
#include "xinput.h"

export module game.input;

export using KeyCode = Keyboard_Keys;
export using XButtonCode = XButton;

export constexpr int VIBRATION_MAX = 65535;
export constexpr int VIBRATION_MIN = 0;

export constexpr int VIBRATION_HIGH = static_cast<int>(VIBRATION_MAX * 0.75f);
export constexpr int VIBRATION_HALF = static_cast<int>(VIBRATION_MAX * 0.5f);
export constexpr int VIBRATION_UPPER_LOW = static_cast<int>(VIBRATION_MAX * 0.4f);
export constexpr int VIBRATION_LOW = static_cast<int>(VIBRATION_MAX * 0.25f);
export constexpr int VIBRATION_LOWER_LOW = static_cast<int>(VIBRATION_MAX * 0.1f);

export class InputHandler {
private:

public:
  InputHandler();
  void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

  void OnUpdate();
  bool IsKeyDown(KeyCode key_code);
  bool IsKeyUp(KeyCode key_code);
  bool GetKey(KeyCode key_code);

  bool IsXInputConnected();
  bool IsXInputButtonDown(XButton button);
  bool IsXInputButtonUp(XButton button);
  bool GetXInputButton(XButton button);
  std::pair<std::pair<float, float>, std::pair<float, float>> GetXInputAnalog();

  void SetXInputVibration(int leftMotor, int rightMotor);
  void StopXInputVibration();
};
