#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>

import graphic.application;

int APIENTRY WinMain(_In_ HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  Application& app = Application::Instance();
  if (!app.Init()) {
    return -1;
  }

  app.Run();
  app.Terminate();

  return 0;
}
