module;
#include "key_logger.h"
#include "xinput.h"
#include <iostream>

module game.input;

InputHandler::InputHandler() {
  KeyLogger_Initialize();
  XInputLogger_Initialize(0);
}

void InputHandler::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  Keyboard_ProcessMessage(message, wParam, lParam);
}

void InputHandler::OnUpdate() {
  KeyLogger_Update();
  XInputLogger_Update();
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

bool InputHandler::IsXInputConnected() {
  return XInputLogger_IsConnected();
}

bool InputHandler::IsXInputButtonDown(XButton button) {
  return XInputLogger_IsTrigger(button);
}

bool InputHandler::IsXInputButtonUp(XButton button) {
  return XInputLogger_IsRelease(button);
}

bool InputHandler::GetXInputButton(XButton button) {
  return XInputLogger_IsPressed(button);
}

/**
 * @brief Retrieve analog stick values from the XInput controller
 *
 * @return std::pair<std::pair<float, float>, std::pair<float, float>> \n
 *         The first pair is the left stick (x, y), the second pair is the right stick (x, y).
 */
std::pair<std::pair<float, float>, std::pair<float, float>> InputHandler::GetXInputAnalog() {
  ControllerAnalog ca = XInputLogger_GetAnalog();
  return std::make_pair(std::make_pair(ca.leftStick.x, ca.leftStick.y), std::make_pair(ca.rightStick.x, ca.rightStick.y)
  );
}
