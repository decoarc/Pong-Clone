#ifndef MENU_H
#define MENU_H

#include <windows.h>
#include "game_state.h"

namespace Menu {
  void drawMenu(HDC hdc, HWND hwnd, const GameState& game);
  bool handleClick(int x, int y, GameState& game);
  void handleKeyDown(WPARAM wParam, GameState& game, HWND hwnd);
  void handleCharInput(char c, GameState& game);
}

#endif

