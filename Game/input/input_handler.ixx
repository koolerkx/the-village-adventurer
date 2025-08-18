module;
#include "key_logger.h"

export module game.input;

export using KeyCode = Keyboard_Keys;

export class InputHandler {
private:

public:
  InputHandler();
  void ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
  
  void OnUpdate();
  bool IsKeyDown(KeyCode key_code);
  bool IsKeyUp(KeyCode key_code);
  bool GetKey(KeyCode key_code);
};


