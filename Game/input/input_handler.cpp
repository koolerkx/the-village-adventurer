module;
#include "key_logger.h"

module game.input;

InputHandler::InputHandler() {
  KeyLogger_Initialize();
}

void InputHandler::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  Keyboard_ProcessMessage(message, wParam, lParam);
}

void InputHandler::OnUpdate() {
  KeyLogger_Update();
}

bool InputHandler::IsKeyDown(KeyCode key_code) {
  return KeyLogger_IsTrigger(key_code);
}

bool InputHandler::IsKeyUp(KeyCode key_code) {
  return KeyLogger_IsRelease(key_code);
}

bool InputHandler::GetKey(KeyCode key_code) {
  return KeyLogger_IsPressed(key_code);
}
