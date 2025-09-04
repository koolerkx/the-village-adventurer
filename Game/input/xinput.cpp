#include "xinput.h"
#include <algorithm>

namespace {
  DWORD g_userIndex = 0;
  bool g_connected = false;

  XINPUT_STATE g_rawState{};

  std::uint32_t g_nowButtons = 0;
  std::uint32_t g_prevButtons = 0;
  std::uint32_t g_triggerButtons = 0;
  std::uint32_t g_releaseButtons = 0;

  ControllerAnalog g_analog{};

  WORD g_leftThumbDZ = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;   // 7849（SDK default）
  WORD g_rightThumbDZ = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE; // 8689（SDK default）
  BYTE g_triggerThresh = 30;                                 // ~0.12（255 scale）

  inline float normalizeThumb(SHORT v, SHORT deadzone) {
    const int max = 32767;
    int av = std::abs((int)v);
    if (av <= deadzone) return 0.0f;
    // map to [0,1]
    float norm = static_cast<float>(av - deadzone) / static_cast<float>(max - deadzone);
    if (norm > 1.0f) norm = 1.0f;
    return (v < 0 ? -norm : norm);
  }

  inline float normalizeTrigger(BYTE v, BYTE threshold) {
    if (v <= threshold) return 0.0f;
    float norm = float(v - threshold) / float(255 - threshold);
    return std::clamp(norm, 0.0f, 1.0f);
  }

  inline std::uint32_t mapButtonsWithVirtual(const XINPUT_GAMEPAD& gp, float ltNorm, float rtNorm) {
    std::uint32_t mask = gp.wButtons;
    if (ltNorm > 0.0f) mask |= static_cast<std::uint32_t>(XButton::LT);
    if (rtNorm > 0.0f) mask |= static_cast<std::uint32_t>(XButton::RT);
    return mask;
  }
}

void XInputLogger_Initialize(DWORD userIndex) {
  g_userIndex = userIndex;
  g_connected = false;
  std::memset(&g_rawState, 0, sizeof(g_rawState));
  g_nowButtons = g_prevButtons = g_triggerButtons = g_releaseButtons = 0;
  g_analog = ControllerAnalog{};
}

void XInputLogger_Update() {
  XINPUT_STATE state{};
  DWORD res = XInputGetState(g_userIndex, &state);

  if (res == ERROR_SUCCESS) {
    g_connected = true;
    g_rawState = state;

    const float lt = normalizeTrigger(state.Gamepad.bLeftTrigger, g_triggerThresh);
    const float rt = normalizeTrigger(state.Gamepad.bRightTrigger, g_triggerThresh);
    const float lx = normalizeThumb(state.Gamepad.sThumbLX, static_cast<SHORT>(g_leftThumbDZ));
    const float ly = normalizeThumb(state.Gamepad.sThumbLY, static_cast<SHORT>(g_leftThumbDZ));
    const float rx = normalizeThumb(state.Gamepad.sThumbRX, static_cast<SHORT>(g_rightThumbDZ));
    const float ry = normalizeThumb(state.Gamepad.sThumbRY, static_cast<SHORT>(g_rightThumbDZ));
    g_analog = ControllerAnalog{lt, rt, {lx, ly}, {rx, ry}};

    std::uint32_t now = mapButtonsWithVirtual(state.Gamepad, lt, rt);

    g_triggerButtons = (g_prevButtons ^ now) & now;
    g_releaseButtons = (g_prevButtons ^ now) & g_prevButtons;
    g_prevButtons = g_nowButtons = now;
  }
  else {
    g_connected = false;
    std::memset(&g_rawState, 0, sizeof(g_rawState));
    g_analog = ControllerAnalog{};
    g_triggerButtons = g_releaseButtons = 0;
    g_prevButtons = g_nowButtons = 0;
  }
}

bool XInputLogger_IsConnected() { return g_connected; }

static inline std::uint32_t toMask(XButton b) {
  return static_cast<std::uint32_t>(b);
}

bool XInputLogger_IsPressed(XButton b) {
  return (g_nowButtons & toMask(b)) != 0u;
}

bool XInputLogger_IsTrigger(XButton b) {
  return (g_triggerButtons & toMask(b)) != 0u;
}

bool XInputLogger_IsRelease(XButton b) {
  return (g_releaseButtons & toMask(b)) != 0u;
}

ControllerAnalog XInputLogger_GetAnalog() { return g_analog; }

void XInputLogger_SetDeadZones(WORD leftThumbDZ, WORD rightThumbDZ, BYTE triggerThreshold) {
  g_leftThumbDZ = leftThumbDZ;
  g_rightThumbDZ = rightThumbDZ;
  g_triggerThresh = triggerThreshold;
}

DWORD XInputLogger_GetUserIndex() { return g_userIndex; }
void XInputLogger_SetUserIndex(DWORD idx) { g_userIndex = idx; }

XINPUT_STATE XInputLogger_GetRawState() { return g_rawState; }
