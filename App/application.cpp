module;
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>

module application;

import std;
import graphic.direct3D;
import graphic.utils.math;
import game.scene;
import game.title_scene;

// Load from config
static constexpr char WINDOW_CLASS[] = "GameWindow"; // メインウインドウクラス名
static constexpr char TITLE[] = "Game";              // 	タイトルバーのテクスト
constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

LRESULT CALLBACK WindowProcedure(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam) {
  switch (message) {
  case WM_KEYDOWN:
    // https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
    if (wParam == VK_ESCAPE) {
      SendMessage(hWnd, WM_CLOSE, 0, 0);
    }
    break;
  case WM_CLOSE:
    // TODO: Load from config file
    if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_YESNO | MB_DEFBUTTON2) == IDYES) {
      DestroyWindow(hWnd);
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void Application::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass) {
  HINSTANCE hinst = GetModuleHandle(nullptr);

  // ウインドウクラスの登録
  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.lpfnWndProc = WindowProcedure;
  windowClass.hInstance = hinst;
  windowClass.hIcon = LoadIcon(hinst, IDI_APPLICATION);
  windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
  windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  windowClass.lpszMenuName = nullptr;
  windowClass.lpszClassName = WINDOW_CLASS;
  windowClass.hIconSm = LoadIcon(hinst, IDI_APPLICATION);

  RegisterClassEx(&windowClass);

  // メインウィンドウの作成

  RECT window_rect{
    0, 0, WINDOW_WIDTH, WINDOW_HEIGHT
  };

  DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

  AdjustWindowRect(&window_rect, style, FALSE);

  const int window_width = window_rect.right - window_rect.left;
  const int window_height = window_rect.bottom - window_rect.top;

  // デスクトップのサイズを取得
  // ライブラリモニターの画面解像度取得
  const int desktop_width = GetSystemMetrics(SM_CXSCREEN);
  const int desktop_height = GetSystemMetrics(SM_CYSCREEN);

  // ウインドウの表示位置をデスクトップの真ん中に調整する
  // offset to center, use std::max to prevent window overflow
  const int window_x = std::max(0, (desktop_width - window_width) / 2);
  const int window_y = std::max(0, (desktop_height - window_height) / 2);

  hwnd = CreateWindow(
    WINDOW_CLASS,
    TITLE,
    style,
    window_x,
    window_y,
    window_width,
    window_height,
    nullptr,
    nullptr,
    hinst,
    nullptr
  );
}

SIZE Application::GetWindowSize() {
  SIZE ret;
  ret.cx = WINDOW_WIDTH;
  ret.cy = WINDOW_HEIGHT;
  return ret;
}

void Application::Run() const {
  ShowWindow(hwnd_, SW_SHOW);
  UpdateWindow(hwnd_);

  MSG msg = {};
  do {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      // todo: add delta time
      scene_manager_->OnUpdate(0.0f);
      debug_manager_->OnUpdate(0.0f);

      direct3d_->BeginDraw();
      scene_manager_->OnRender();
      // direct3d_->Dispatch(on_update_function);

      debug_manager_->OnRender();
      direct3d_->EndDraw();
    }
  }
  while (msg.message != WM_QUIT);
}

bool Application::Init() {
  (void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  CreateGameWindow(hwnd_, window_class_);

  direct3d_.reset(new Dx11Wrapper(hwnd_));

  std::unique_ptr<GameContext> initial_context = std::make_unique<GameContext>();
  initial_context->render_resource_manager = direct3d_->GetResourceManager();

  scene_manager_.reset(
    new SceneManager(std::move(
                       std::make_unique<TitleScene>()
                     ),
                     std::move(initial_context)
    )
  );

  std::unique_ptr<DebugContext> debug_context = std::make_unique<DebugContext>();
  debug_context->render_resource_manager = direct3d_->GetResourceManager();
  debug_context->window_size = GetWindowSize();
  debug_manager_.reset(new DebugManager(std::move(debug_context)));

  return true;
}

void Application::Terminate() const {
  UnregisterClass(window_class_.lpszClassName, window_class_.hInstance);
}

Application& Application::Instance() {
  static Application instance;
  return instance;
}

Application::Application() {}

Application::~Application() {}
