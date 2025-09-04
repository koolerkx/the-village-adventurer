module;
#include "key_logger.h"
#include "xinput.h"

export module game.input;

export using KeyCode = Keyboard_Keys;
export using XButtonCode = XButton;

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
};
