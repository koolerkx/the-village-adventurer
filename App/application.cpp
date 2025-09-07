module;
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <objbase.h>
#include "resource.h"

module application;

import std;
import app.timer;
import app.loader.yaml;
import graphic.direct3D;
import graphic.utils.types;
import game.scene;
import game.scene.splash_scene;

// config
constexpr const char* config_filepath = "./config.yaml";

// Load from config
static constexpr char WINDOW_CLASS[] = "村の冒険者 | The Village Adventurer"; // メインウインドウクラス名
static constexpr char TITLE[] = "村の冒険者 | The Village Adventurer";              // 	タイトルバーのテクスト
// constexpr unsigned int WINDOW_WIDTH = 1280;
// constexpr unsigned int WINDOW_HEIGHT = 720;

LRESULT CALLBACK Application::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_NCCREATE) {
    CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
    Application* pApp = reinterpret_cast<Application*>(pCreate->lpCreateParams);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pApp));
    return DefWindowProc(hWnd, message, wParam, lParam);
  }

  Application* pApp = reinterpret_cast<Application*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  if (pApp) {
    return pApp->HandleWindowMessage(hWnd, message, wParam, lParam);
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Application::HandleWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_CLOSE:
    DestroyWindow(hWnd);
    // if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_YESNO | MB_DEFBUTTON2) == IDYES) {
    //   DestroyWindow(hWnd);
    // }
    break;
  case WM_ACTIVATEAPP:
  case WM_KEYDOWN:
  // https://learn.microsoft.com/ja-jp/windows/win32/inputdev/virtual-key-codes
  // if (wParam == VK_ESCAPE) {
  //   SendMessage(hWnd, WM_CLOSE, 0, 0);
  // }
  case WM_SYSKEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYUP:
    input_handler_->ProcessMessage(message, wParam, lParam);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

void Application::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass, const GraphicConfig& graphic_config) {
  HINSTANCE hinst = GetModuleHandle(nullptr);
  win_size_ = {graphic_config.window_size_width, graphic_config.window_size_height};

  // ウインドウクラスの登録
  windowClass.cbSize = sizeof(WNDCLASSEX);
  windowClass.lpfnWndProc = WindowProcedure;
  windowClass.hInstance = hinst;
  windowClass.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_APP_ICON));
  windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
  windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  windowClass.lpszMenuName = nullptr;
  windowClass.lpszClassName = WINDOW_CLASS;
  windowClass.hIconSm = LoadIcon(hinst, IDI_APPLICATION);

  RegisterClassEx(&windowClass);

  // メインウィンドウの作成

  RECT window_rect{
    0, 0, win_size_.cx, win_size_.cy
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
    this
  );
}

SIZE Application::GetWindowSize() {
  return win_size_;
}

bool Application::Init() {
  (void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  // load config
  YAMLConfigLoader config_loader{config_filepath};
  const Config config = config_loader.getConfig();

  if (config.graphic.handle_dps) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  }

  CreateGameWindow(hwnd_, window_class_, config.graphic);

  direct3d_.reset(new Dx11Wrapper(hwnd_, {
                                    .vsync{config.graphic.vsync},
                                    .window_size_width{config.graphic.window_size_width},
                                    .window_size_height{config.graphic.window_size_height},
                                    .vertex_shader = config.graphic.shader_files.vertex_shader,
                                    .instanced_vertex_shader = config.graphic.shader_files.instanced_vertex_shader,
                                    .pixel_shader = config.graphic.shader_files.pixel_shader,
                                    .swirl_pixel_shader = config.graphic.shader_files.swirl_pixel_shader
                                  }));

  std::unique_ptr<GameContext> initial_context = std::make_unique<GameContext>();
  initial_context->render_resource_manager = direct3d_->GetResourceManager();
  initial_context->input_handler = input_handler_.get();
  initial_context->window_width = config.graphic.window_size_width;
  initial_context->window_height = config.graphic.window_size_height;

  SceneManager::Init(std::move(
                       std::make_unique<SplashScene>()
                     ),
                     std::move(initial_context),
                     std::make_unique<GameConfig>(GameConfig{
                         .default_map = config.map.default_map,
                         .map_texture_filepath = config.map.map_texture,
                         .map_tile_filepath = config.map.map_metadata,
                         .file_paths = config.map.files
                       }
                     )
  );

#if defined(DEBUG) || defined(_DEBUG)
  std::unique_ptr<DebugContext> debug_context = std::make_unique<DebugContext>();
  debug_context->render_resource_manager = direct3d_->GetResourceManager();
  debug_context->window_size = GetWindowSize();
  debug_manager_.reset(new DebugManager(std::move(debug_context)));
#endif

  timer_updater_.reset(new TimerUpdater(60.0f));
  input_handler_.reset(new InputHandler());

  return true;
}

void Application::Run() {
  ShowWindow(hwnd_, SW_SHOW);
  UpdateWindow(hwnd_);

  auto updateFn = [this](float dt) { OnUpdate(dt); };
  auto fixedFn = [this](float fdt) { OnFixedUpdate(fdt); };

  MSG msg = {};
  do {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      timer_updater_->tick(updateFn, fixedFn);
    }
  }
  while (msg.message != WM_QUIT);
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

void Application::OnUpdate(float delta_time) {
  input_handler_->OnUpdate();

  if (input_handler_->IsKeyDown(KeyCode::KK_F11)) {
    if (window_state_ == BORDERLESS) {
      ExitBorderless();
      window_state_ = WINDOWED;
      direct3d_->SetFullscreen(false);
    }
    else if (window_state_ == WINDOWED) {
      window_state_ = FULLSCREEN;
      direct3d_->SetFullscreen(true);
    }
    else {
      EnterBorderless();
    }
  }

  SceneManager::GetInstance().OnUpdate(delta_time);

#if defined(DEBUG) || defined(_DEBUG)
  debug_manager_->OnUpdate(delta_time);
#endif

  direct3d_->BeginDraw();
  SceneManager::GetInstance().OnRender();
  // direct3d_->Dispatch(on_update_function);

#if defined(DEBUG) || defined(_DEBUG)
  debug_manager_->OnRender();
#endif
  direct3d_->EndDraw();

  if (SceneManager::GetInstance().IsLeave()) {
    SendMessage(hwnd_, WM_CLOSE, 0, 0);
    SceneManager::GetInstance().SetLeave(false);
  }
}

void Application::OnFixedUpdate(float delta_time) {
  SceneManager::GetInstance().OnFixedUpdate(delta_time);

#if defined(DEBUG) || defined(_DEBUG)
  debug_manager_->OnFixedUpdate(delta_time);
#endif
}


static RECT GetMonitorRectForWindow(HWND hwnd) {
  HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  MONITORINFO mi{sizeof(mi)};
  GetMonitorInfo(hmon, &mi);
  return mi.rcMonitor;
}

void Application::EnterBorderless() {
  if (window_state_ == WindowState::BORDERLESS) return;
  direct3d_->SetFullscreen(false);

  saved_style_ = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
  saved_ex_style_ = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

  WINDOWPLACEMENT wp{sizeof(wp)};
  GetWindowPlacement(hwnd_, &wp);
  saved_maximized_ = (wp.showCmd == SW_SHOWMAXIMIZED);

  GetWindowRect(hwnd_, &saved_rect_);

  // Set borderless
  SetWindowLongPtr(hwnd_, GWL_STYLE, WS_POPUP | WS_VISIBLE);
  SetWindowLongPtr(hwnd_, GWL_EXSTYLE, saved_ex_style_ & ~(WS_EX_WINDOWEDGE));

  RECT mon = GetMonitorRectForWindow(hwnd_);

  // Always on top
  const HWND zOrder = HWND_TOP;
  SetWindowPos(hwnd_, zOrder,
               mon.left, mon.top,
               mon.right - mon.left, mon.bottom - mon.top,
               SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

  // note: remain direct3d canvas resolution 
  // direct3d_->Resize(mon.right - mon.left, mon.bottom - mon.top);

  window_state_ = WindowState::BORDERLESS;
}

void Application::ExitBorderless() {
  if (window_state_ != WindowState::BORDERLESS) return;

  SetWindowLongPtr(hwnd_, GWL_STYLE, saved_style_);
  SetWindowLongPtr(hwnd_, GWL_EXSTYLE, saved_ex_style_);

  SetWindowPos(hwnd_, nullptr,
               saved_rect_.left, saved_rect_.top,
               saved_rect_.right - saved_rect_.left,
               saved_rect_.bottom - saved_rect_.top,
               SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_SHOWWINDOW);

  if (saved_maximized_) {
    ShowWindow(hwnd_, SW_MAXIMIZE);
  }
  else {
    ShowWindow(hwnd_, SW_SHOWNOACTIVATE);
  }

  window_state_ = WindowState::WINDOWED;
}
