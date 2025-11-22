#include "window.h"
#include "constants.h"
#include "game_logic.h"
#include "renderer.h"
#include "menu.h"
#include "network.h"
#include <algorithm>
#include <cstdlib>
#include <string>

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
            // Processar rede
            Network::update();
            
            if (g_gameState->isOnlineMultiplayer) {
              if (Network::getRole() == NetworkRole::HOST) {
                // Host: processar mensagens e enviar estado
                Network::processIncomingMessages(*g_gameState);
                GameLogic::updateGame(*g_gameState);
                Network::sendGameState(*g_gameState);
              } else if (Network::getRole() == NetworkRole::CLIENT) {
                // Cliente: enviar input e receber estado
                Network::sendInput(g_gameState->rightPaddleUp, g_gameState->rightPaddleDown);
                Network::receiveGameState(*g_gameState);
                
                // Cliente também processa seu próprio paddle esquerdo
                if (g_gameState->leftPaddleUp) g_gameState->leftPaddleY -= 1;
                if (g_gameState->leftPaddleDown) g_gameState->leftPaddleY += 1;
                
                int minPaddleY = Constants::kTopOffset + 1;
                int maxPaddleY = Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight - 1;
                if (g_gameState->leftPaddleY < minPaddleY) g_gameState->leftPaddleY = minPaddleY;
                if (g_gameState->leftPaddleY > maxPaddleY) g_gameState->leftPaddleY = maxPaddleY;
              }
            } else {
              // Modo local
              GameLogic::updateGame(*g_gameState);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
          } else if (g_gameState->mode == GameMode::HOST_WAITING) {
            // Host aguardando - verificar se cliente conectou
            Network::update();
            Network::processIncomingMessages(*g_gameState);
            
            if (Network::isClientConnected()) {
              // Cliente conectou, iniciar jogo
              g_gameState->mode = GameMode::PLAYING;
              // Reset game state
              g_gameState->leftPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
              g_gameState->rightPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
              g_gameState->leftScore = 0;
              g_gameState->rightScore = 0;
              g_gameState->paused = false;
              GameLogic::resetBall(*g_gameState, (rand() % 2 == 0) ? -1 : 1);
              SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
            }
            
            // Forçar redesenho para atualizar display do IP
            InvalidateRect(hwnd, nullptr, FALSE);
          } else if (g_gameState->mode == GameMode::CLIENT_CONNECTING) {
            // Forçar redesenho periódico para cursor piscante
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
          GameMode oldMode = g_gameState->mode;
          Menu::handleKeyDown(wParam, *g_gameState, hwnd);
          
          // Se mudou para HOST_WAITING ou CLIENT_CONNECTING, iniciar timer
          if (oldMode == GameMode::MENU) {
            if (g_gameState->mode == GameMode::HOST_WAITING || g_gameState->mode == GameMode::CLIENT_CONNECTING) {
              SetTimer(hwnd, *g_timerId, 100, nullptr);
            }
          }
          
          if (g_gameState->mode == GameMode::PLAYING) {
            // Reset game state when starting
            g_gameState->leftPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
            g_gameState->rightPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
            g_gameState->leftScore = 0;
            g_gameState->rightScore = 0;
            g_gameState->paused = false;
            g_gameState->leftPaddleUp = false;
            g_gameState->leftPaddleDown = false;
            g_gameState->rightPaddleUp = false;
            g_gameState->rightPaddleDown = false;
            // Reset AI state
            if (!g_gameState->isOnlineMultiplayer) {
              g_gameState->aiConsecutiveHits = 0;
              g_gameState->aiReactionDelayCounter = 0;
              g_gameState->aiCurrentReactionDelay = 0;
            }
            // Reset ball with random direction
            GameLogic::resetBall(*g_gameState, (rand() % 2 == 0) ? -1 : 1);
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
              // Cliente controla paddle direito no modo online
              if (g_gameState->isOnlineMultiplayer && Network::getRole() == NetworkRole::CLIENT) {
                g_gameState->rightPaddleUp = true;
              } else if (!g_gameState->isSinglePlayer) {
                g_gameState->rightPaddleUp = true;
              }
              break;
            case VK_DOWN:
              // Cliente controla paddle direito no modo online
              if (g_gameState->isOnlineMultiplayer && Network::getRole() == NetworkRole::CLIENT) {
                g_gameState->rightPaddleDown = true;
              } else if (!g_gameState->isSinglePlayer) {
                g_gameState->rightPaddleDown = true;
              }
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
              // Limpar conexão de rede
              if (g_gameState->isOnlineMultiplayer) {
                if (Network::getRole() == NetworkRole::HOST) {
                  Network::stopHost();
                } else if (Network::getRole() == NetworkRole::CLIENT) {
                  Network::disconnect();
                }
                g_gameState->isOnlineMultiplayer = false;
              }
              // Se estiver em HOST_WAITING ou CLIENT_CONNECTING, voltar ao menu
              if (g_gameState->mode == GameMode::HOST_WAITING || g_gameState->mode == GameMode::CLIENT_CONNECTING) {
                g_gameState->mode = GameMode::MENU;
                g_gameState->hostIPInput = "";
                g_gameState->connectionStatus = "";
              } else {
                g_gameState->mode = GameMode::MENU;
              }
              // Reset paddle states when returning to menu
              g_gameState->leftPaddleUp = false;
              g_gameState->leftPaddleDown = false;
              g_gameState->rightPaddleUp = false;
              g_gameState->rightPaddleDown = false;
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
              // Cliente controla paddle direito no modo online
              if (g_gameState->isOnlineMultiplayer && Network::getRole() == NetworkRole::CLIENT) {
                g_gameState->rightPaddleUp = false;
              } else if (!g_gameState->isSinglePlayer) {
                g_gameState->rightPaddleUp = false;
              }
              break;
            case VK_DOWN:
              // Cliente controla paddle direito no modo online
              if (g_gameState->isOnlineMultiplayer && Network::getRole() == NetworkRole::CLIENT) {
                g_gameState->rightPaddleDown = false;
              } else if (!g_gameState->isSinglePlayer) {
                g_gameState->rightPaddleDown = false;
              }
              break;
          }
        }
        return 0;

      case WM_CHAR:
        if (g_gameState->mode == GameMode::CLIENT_CONNECTING) {
          char c = (char)wParam;
          Menu::handleCharInput(c, *g_gameState);
          // Se conectou, o handleCharInput já mudou o modo
          if (g_gameState->mode != GameMode::CLIENT_CONNECTING) {
            // Parar timer de cursor piscante e iniciar timer do jogo
            KillTimer(hwnd, *g_timerId);
            SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
          }
          InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

      case WM_LBUTTONDOWN:
        if (g_gameState->mode == GameMode::MENU) {
          int x = LOWORD(lParam);
          int y = HIWORD(lParam);
          GameMode oldMode = g_gameState->mode;
          if (!Menu::handleClick(x, y, *g_gameState)) {
            PostMessage(hwnd, WM_CLOSE, 0, 0);
          } else {
            // Se mudou para HOST_WAITING ou CLIENT_CONNECTING, iniciar timer
            if (oldMode == GameMode::MENU) {
              if (g_gameState->mode == GameMode::HOST_WAITING || g_gameState->mode == GameMode::CLIENT_CONNECTING) {
                SetTimer(hwnd, *g_timerId, 100, nullptr);
              }
            }
            
            if (g_gameState->mode == GameMode::PLAYING) {
              // Reset game state when starting
              g_gameState->leftPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
              g_gameState->rightPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
              g_gameState->leftScore = 0;
              g_gameState->rightScore = 0;
              g_gameState->paused = false;
              g_gameState->leftPaddleUp = false;
              g_gameState->leftPaddleDown = false;
              g_gameState->rightPaddleUp = false;
              g_gameState->rightPaddleDown = false;
              // Reset AI state
              if (!g_gameState->isOnlineMultiplayer) {
                g_gameState->aiConsecutiveHits = 0;
                g_gameState->aiReactionDelayCounter = 0;
                g_gameState->aiCurrentReactionDelay = 0;
              }
              // Reset ball with random direction
              GameLogic::resetBall(*g_gameState, (rand() % 2 == 0) ? -1 : 1);
              SetTimer(hwnd, *g_timerId, g_gameState->frameDelayMs, nullptr);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
          }
        }
        return 0;

      case WM_CLOSE:
        KillTimer(hwnd, *g_timerId);
        // Limpar conexão de rede
        if (g_gameState->isOnlineMultiplayer) {
          if (Network::getRole() == NetworkRole::HOST) {
            Network::stopHost();
          } else if (Network::getRole() == NetworkRole::CLIENT) {
            Network::disconnect();
          }
        }
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

