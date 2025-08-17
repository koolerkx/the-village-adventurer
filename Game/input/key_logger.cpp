#include "keyboard.h"

static Keyboard_State g_PrevState;
static Keyboard_State g_TriggerState;
static Keyboard_State g_ReleaseState;

void KeyLogger_Initialize()
{
    Keyboard_Initialize();
}

void KeyLogger_Update()
{
    const Keyboard_State* pState = Keyboard_GetState();
    LPBYTE pNowState = (LPBYTE)pState;
    LPBYTE pPreviewState = (LPBYTE)&g_PrevState;
    LPBYTE pTriggerState = (LPBYTE)&g_TriggerState;
    LPBYTE pReleaseState = (LPBYTE)&g_ReleaseState;

    for (int i = 0; i < sizeof(Keyboard_State); i++)
    {
        pTriggerState[i] = (pPreviewState[i] ^ pNowState[i]) & pNowState[i];
        pReleaseState[i] = (pPreviewState[i] ^ pNowState[i]) & pPreviewState[i];
    }

    g_PrevState = *pState;
}

bool KeyLogger_IsPressed(Keyboard_Keys key)
{
    return Keyboard_IsKeyDown(key);
}

bool KeyLogger_IsTrigger(Keyboard_Keys key)
{
    return Keyboard_IsKeyDown(key, &g_TriggerState);
}

bool KeyLogger_IsRelease(Keyboard_Keys key)
{
    return Keyboard_IsKeyDown(key, &g_ReleaseState);
}
