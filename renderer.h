#ifndef RENDERER_H
#define RENDERER_H

#include <windows.h>
#include "game_state.h"

namespace Renderer {
  void drawGame(HDC hdc, HWND hwnd, const GameState& game);
}

#endif

