#include <windows.h>
#include "constants.h"
#include "game_state.h"
#include "window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  GameState game;
  UINT_PTR timerId = 1;

  HWND hwnd = Window::createWindow(hInstance, nCmdShow, game, timerId);
  if (hwnd == nullptr) {
    return 0;
  }

  // Message loop
  MSG msg = {};
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

