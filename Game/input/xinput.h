#pragma once
#include <cstdint>
#include <Windows.h>
#include <Xinput.h>

#pragma comment(lib, "Xinput.lib")

enum class XButton : std::uint32_t {
  DPadUp     = XINPUT_GAMEPAD_DPAD_UP,
  DPadDown   = XINPUT_GAMEPAD_DPAD_DOWN,
  DPadLeft   = XINPUT_GAMEPAD_DPAD_LEFT,
  DPadRight  = XINPUT_GAMEPAD_DPAD_RIGHT,
  Start      = XINPUT_GAMEPAD_START,
  Back       = XINPUT_GAMEPAD_BACK,
  LeftThumb  = XINPUT_GAMEPAD_LEFT_THUMB,
  RightThumb = XINPUT_GAMEPAD_RIGHT_THUMB,
  LB         = XINPUT_GAMEPAD_LEFT_SHOULDER,
  RB         = XINPUT_GAMEPAD_RIGHT_SHOULDER,
  A          = XINPUT_GAMEPAD_A,
  B          = XINPUT_GAMEPAD_B,
  X          = XINPUT_GAMEPAD_X,
  Y          = XINPUT_GAMEPAD_Y,

  LT = 0x00010000u,
  RT = 0x00020000u,
};


struct Thumbstick {
  float x = 0.0f;
  float y = 0.0f;
}; // [-1, 1]，deadzone


struct ControllerAnalog {
  float leftTrigger = 0.0f;  // [0, 1]
  float rightTrigger = 0.0f; // [0, 1]
  Thumbstick leftStick{};    // [-1, 1]
  Thumbstick rightStick{};   // [-1, 1]
};

void XInputLogger_Initialize(DWORD userIndex = 0);
void XInputLogger_Update();

bool XInputLogger_IsConnected();

bool XInputLogger_IsPressed(XButton b);
bool XInputLogger_IsTrigger(XButton b);
bool XInputLogger_IsRelease(XButton b);

ControllerAnalog XInputLogger_GetAnalog();

void XInputLogger_SetDeadZones(WORD leftThumbDZ, WORD rightThumbDZ, BYTE triggerThreshold = 30);

DWORD XInputLogger_GetUserIndex();
void XInputLogger_SetUserIndex(DWORD idx);

XINPUT_STATE XInputLogger_GetRawState();
