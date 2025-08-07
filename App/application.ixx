module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module application;

import std;
import graphic.direct3D;

export class Application {
private:
  // ウィンドウ
  WNDCLASSEX window_class_;
  HWND hwnd_ = nullptr;
  std::shared_ptr<Dx11Wrapper> direct3d_;

  // ゲーム用ウィンドウの生成
  static void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);

  // シングルトン、コピー・代入禁止
  Application();
  Application(const Application&) = delete;
  void operator=(const Application&) = delete;

public:
  static Application& Instance();

  bool Init();      /// 初期化
  void Run() const; /// ループ起動

  /// 後処理
  void Terminate() const;
  static SIZE GetWindowSize();
  ~Application();
};
