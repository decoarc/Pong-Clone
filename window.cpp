#include "window.h"
#include "constants.h"
#include "game_logic.h"
#include "renderer.h"
#include "menu.h"
#include <algorithm>

namespace Window {
  static GameState* g_gameState = nullptr;
  static UINT_PTR* g_timerId = nullptr;

  LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
      case WM_CREATE:
        return 0;

      case WM_TIMER:
        if (wParam == *g_timerId) {
          if (g_gameState->mode == GameMode::PLAYING) {
            GameLogic::updateGame(*g_gameState);
            InvalidateRect(hwnd, nullptr, FALSE);
          }
        }
        return 0;

      case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        RECT rect;
        GetClientRect(hwnd, &rect);
        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
        
        Renderer::draw(hdcMem, hwnd, *g_gameState);
        
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);
        
        SelectObject(hdcMem, hbmOld);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        
        EndPaint(hwnd, &ps);
        return 0;
      }

      case WM_KEYDOWN:
        if (g_gameState->mode == GameMode::MENU) {
          Menu::handleKeyDown(wParam, *g_gameState, hwnd);
          
          if (g_gameState->mode == GameMode::PLAYING) {
            SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
          }
        } else {
          switch (wParam) {
            case 'W':
            case 'w':
              g_gameState->leftPaddleUp = true;
              break;
            case 'S':
            case 's':
              g_gameState->leftPaddleDown = true;
              break;
            case VK_UP:
              g_gameState->rightPaddleUp = true;
              break;
            case VK_DOWN:
              g_gameState->rightPaddleDown = true;
              break;
            case 'P':
            case 'p':
              g_gameState->paused = !g_gameState->paused;
              InvalidateRect(hwnd, nullptr, FALSE);
              break;
            case VK_OEM_PLUS:
            case VK_ADD:
              g_gameState->frameDelayMs = std::max(1, g_gameState->frameDelayMs - 1);
              KillTimer(hwnd, *g_timerId);
              SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
              break;
            case VK_OEM_MINUS:
            case VK_SUBTRACT:
              g_gameState->frameDelayMs = std::min(100, g_gameState->frameDelayMs + 1);
              KillTimer(hwnd, *g_timerId);
              SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
              break;
            case VK_ESCAPE:
              KillTimer(hwnd, *g_timerId);
              g_gameState->mode = GameMode::MENU;
              InvalidateRect(hwnd, nullptr, FALSE);
              break;
          }
        }
        return 0;

      case WM_KEYUP:
        if (g_gameState->mode == GameMode::PLAYING) {
          switch (wParam) {
            case 'W':
            case 'w':
              g_gameState->leftPaddleUp = false;
              break;
            case 'S':
            case 's':
              g_gameState->leftPaddleDown = false;
              break;
            case VK_UP:
              g_gameState->rightPaddleUp = false;
              break;
            case VK_DOWN:
              g_gameState->rightPaddleDown = false;
              break;
          }
        }
        return 0;

      case WM_LBUTTONDOWN:
        if (g_gameState->mode == GameMode::MENU) {
          int x = LOWORD(lParam);
          int y = HIWORD(lParam);
          if (!Menu::handleClick(x, y, *g_gameState)) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
          } else {
            if (g_gameState->mode == GameMode::PLAYING) {
              SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
          }
        }
        return 0;

      case WM_CLOSE:
        KillTimer(hwnd, *g_timerId);
        DestroyWindow(hwnd);
        return 0;

      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  HWND createWindow(HINSTANCE hInstance, int nCmdShow, GameState& game, UINT_PTR& timerId) {
    g_gameState = &game;
    g_timerId = &timerId;

    const char CLASS_NAME[] = "PongWindowClass";
    
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClassA(&wc);

    RECT windowRect = {0, 0, Constants::kWindowWidth, Constants::kWindowHeight};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);
    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;
    HWND hwnd = CreateWindowExA(
      0,
      CLASS_NAME,
      "Pong",
      WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT,
      windowWidth, windowHeight,
      nullptr, nullptr, hInstance, nullptr
    );

    if (hwnd != nullptr) {
      ShowWindow(hwnd, nCmdShow);
      UpdateWindow(hwnd);
    }

    return hwnd;
  }
}

