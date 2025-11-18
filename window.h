#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include "game_state.h"

namespace Window {
  LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  HWND createWindow(HINSTANCE hInstance, int nCmdShow, GameState& game, UINT_PTR& timerId);
}

#endif

