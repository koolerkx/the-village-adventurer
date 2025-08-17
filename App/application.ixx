module;
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

export module application;

import std;
import app.input;
import app.timer;
import app.loader.types;
import graphic.direct3D;
import game.scene_manager;

#if defined(DEBUG) || defined(_DEBUG)
import graphic.debug;
#endif

export class Application {
private:
  // ウィンドウ
  WNDCLASSEX window_class_;
  HWND hwnd_ = nullptr;
  std::shared_ptr<Dx11Wrapper> direct3d_;
  SIZE win_size_;

  // ゲーム用ウィンドウの生成
  void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass, const GraphicConfig& graphic_config);
  static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  LRESULT HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  
  // シングルトン、コピー・代入禁止
  Application();
  Application(const Application&) = delete;
  void operator=(const Application&) = delete;

#if defined(DEBUG) || defined(_DEBUG)
  std::unique_ptr<DebugManager> debug_manager_ = nullptr;
#endif

  std::unique_ptr<TimerUpdater> timer_updater_ = nullptr;

  std::unique_ptr<InputHandler> input_handler_ = nullptr;

  void OnUpdate(float delta_time);
  void OnFixedUpdate(float delta_time);

public:
  static Application& Instance();

  bool Init(); /// 初期化
  void Run();  /// ループ起動

  /// 後処理
  void Terminate() const;
  SIZE GetWindowSize();
  ~Application();
};
