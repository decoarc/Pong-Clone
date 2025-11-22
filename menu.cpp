#include "menu.h"
#include "constants.h"
#include "network.h"
#include <string>
#include <sstream>
#include <chrono>

namespace Menu {
  constexpr int kMenuStartY = 200;
  constexpr int kMenuOptionSpacing = 70;
  constexpr int kMenuOptionWidth = 300;
  constexpr int kMenuOptionHeight = 50;

  void drawMenu(HDC hdc, HWND hwnd, const GameState& game) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    HFONT hFontTitle = CreateFontA(
        72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFontTitle);
    
    std::string title = "PONG";
    SIZE titleSize;
    GetTextExtentPoint32A(hdc, title.c_str(), static_cast<int>(title.length()), &titleSize);
    int titleX = (Constants::kWindowWidth - titleSize.cx) / 2;
    int titleY = 100;
    TextOutA(hdc, titleX, titleY, title.c_str(), static_cast<int>(title.length()));
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFontTitle);

    HFONT hFontMenu = CreateFontA(
        48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
    );
    SelectObject(hdc, hFontMenu);

    // SINGLE PLAYER

    int singleplayerY = kMenuStartY;
    bool singleplayerSelected = (game.selectedMenuOption == MenuOption::SINGLEPLAYER);
    SetTextColor(hdc, singleplayerSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));    
    std::string singleplayerText = "SINGLE PLAYER";
    SIZE singleplayerSize;
    GetTextExtentPoint32A(hdc, singleplayerText.c_str(), static_cast<int>(singleplayerText.length()), &singleplayerSize);
    int singleplayerX = (Constants::kWindowWidth - singleplayerSize.cx) / 2;
    TextOutA(hdc, singleplayerX, singleplayerY, singleplayerText.c_str(), static_cast<int>(singleplayerText.length()));

    // MULTIPLAYER LOCAL

    int multiplayerY = kMenuStartY + kMenuOptionSpacing;
    bool multiplayerSelected = (game.selectedMenuOption == MenuOption::MULTIPLAYER);
    SetTextColor(hdc, multiplayerSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));    
    std::string multiplayerText = "MULTIPLAYER";
    SIZE multiplayerSize;
    GetTextExtentPoint32A(hdc, multiplayerText.c_str(), static_cast<int>(multiplayerText.length()), &multiplayerSize);
    int multiplayerX = (Constants::kWindowWidth - multiplayerSize.cx) / 2;
    TextOutA(hdc, multiplayerX, multiplayerY, multiplayerText.c_str(), static_cast<int>(multiplayerText.length()));

    // ONLINE HOST

    int hostY = kMenuStartY + kMenuOptionSpacing * 2;
    bool hostSelected = (game.selectedMenuOption == MenuOption::ONLINE_HOST);
    SetTextColor(hdc, hostSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));
    std::string hostText = "ONLINE HOST";
    SIZE hostSize;
    GetTextExtentPoint32A(hdc, hostText.c_str(), static_cast<int>(hostText.length()), &hostSize);
    int hostX = (Constants::kWindowWidth - hostSize.cx) / 2;
    TextOutA(hdc, hostX, hostY, hostText.c_str(), static_cast<int>(hostText.length()));

     // ONLINE JOIN

    int joinY = kMenuStartY + kMenuOptionSpacing * 3;
    bool joinSelected = (game.selectedMenuOption == MenuOption::ONLINE_JOIN);
    SetTextColor(hdc, joinSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));
    std::string joinText = "ONLINE JOIN";
    SIZE joinSize;
    GetTextExtentPoint32A(hdc, joinText.c_str(), static_cast<int>(joinText.length()), &joinSize);
    int joinX = (Constants::kWindowWidth - joinSize.cx) / 2;
    TextOutA(hdc, joinX, joinY, joinText.c_str(), static_cast<int>(joinText.length()));

    // QUIT

    int quitY = kMenuStartY + kMenuOptionSpacing * 4;
    bool quitSelected = (game.selectedMenuOption == MenuOption::QUIT);
    SetTextColor(hdc, quitSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));    
    std::string quitText = "QUIT";
    SIZE quitSize;
    GetTextExtentPoint32A(hdc, quitText.c_str(), static_cast<int>(quitText.length()), &quitSize);
    int quitX = (Constants::kWindowWidth - quitSize.cx) / 2;
    TextOutA(hdc, quitX, quitY, quitText.c_str(), static_cast<int>(quitText.length()));

    // Se estiver no modo HOST_WAITING ou CLIENT_CONNECTING, desenhar campo destacado no centro
    if (game.mode == GameMode::HOST_WAITING || game.mode == GameMode::CLIENT_CONNECTING) {
      // Desenhar campo destacado no centro da tela
      RECT inputRect;
      inputRect.left = Constants::kWindowWidth / 2 - 250;
      inputRect.top = Constants::kWindowHeight / 2 - 50;
      inputRect.right = Constants::kWindowWidth / 2 + 250;
      inputRect.bottom = Constants::kWindowHeight / 2 + 50;
      
      // Fundo branco para o campo
      HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
      FillRect(hdc, &inputRect, whiteBrush);
      DeleteObject(whiteBrush);
      
      // Borda destacada
      HBRUSH borderBrush = CreateSolidBrush(RGB(0, 255, 255));
      FrameRect(hdc, &inputRect, borderBrush);
      DeleteObject(borderBrush);
      
      // Texto do label
      HFONT hFontLabel = CreateFontA(
          24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
          DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
      );
      HFONT hFontInput = CreateFontA(
          32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
          DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
      );
      
      // Label acima do campo
      SelectObject(hdc, hFontLabel);
      SetBkMode(hdc, TRANSPARENT);
      SetTextColor(hdc, RGB(255, 255, 255));
      
      std::string labelText = "";
      if (game.mode == GameMode::HOST_WAITING) {
        labelText = "Seu IP do Host:";
      } else {
        labelText = "IP do Host:";
      }
      
      SIZE labelSize;
      GetTextExtentPoint32A(hdc, labelText.c_str(), static_cast<int>(labelText.length()), &labelSize);
      int labelX = (Constants::kWindowWidth - labelSize.cx) / 2;
      int labelY = inputRect.top - 40;
      TextOutA(hdc, labelX, labelY, labelText.c_str(), static_cast<int>(labelText.length()));
      
      // Texto dentro do campo
      SelectObject(hdc, hFontInput);
      SetBkMode(hdc, OPAQUE);
      SetBkColor(hdc, RGB(255, 255, 255));
      SetTextColor(hdc, RGB(0, 0, 0));
      
      std::string displayText = "";
      if (game.mode == GameMode::HOST_WAITING) {
        // Mostrar IP do host
        std::string ip = Network::getHostIP();
        displayText = ip + ":" + std::to_string(game.networkPort);
      } else {
        // Modo CLIENT_CONNECTING - campo de entrada
        displayText = game.hostIPInput.empty() ? "Digite o IP aqui..." : game.hostIPInput;
        
        // Adicionar cursor piscante
        static auto lastBlink = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count();
        bool showCursor = (elapsed % 1000) < 500;
        
        if (showCursor) {
          displayText += "_";
        }
      }
      
      SIZE textSize;
      GetTextExtentPoint32A(hdc, displayText.c_str(), static_cast<int>(displayText.length()), &textSize);
      int textX = inputRect.left + 15;
      int textY = inputRect.top + (inputRect.bottom - inputRect.top - textSize.cy) / 2;
      TextOutA(hdc, textX, textY, displayText.c_str(), static_cast<int>(displayText.length()));
      
      SelectObject(hdc, hOldFont);
      DeleteObject(hFontLabel);
      DeleteObject(hFontInput);
      
      // Instruções abaixo do campo
      HFONT hFontSmall = CreateFontA(
          18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
          DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
      );
      SelectObject(hdc, hFontSmall);
      SetBkMode(hdc, TRANSPARENT);
      SetTextColor(hdc, RGB(200, 200, 200));
      
      std::string instruction = "";
      if (game.mode == GameMode::HOST_WAITING) {
        instruction = "Aguardando cliente conectar... Pressione ESC para voltar";
      } else {
        instruction = "Digite o IP do host e pressione Enter (ex: 192.168.1.100 ou 127.0.0.1)";
      }
      
      SIZE instSize;
      GetTextExtentPoint32A(hdc, instruction.c_str(), static_cast<int>(instruction.length()), &instSize);
      int instX = (Constants::kWindowWidth - instSize.cx) / 2;
      int instY = inputRect.bottom + 30;
      TextOutA(hdc, instX, instY, instruction.c_str(), static_cast<int>(instruction.length()));
      
      SelectObject(hdc, hOldFont);
      DeleteObject(hFontSmall);
    } else {
      // Menu normal - mostrar status text
      HFONT hFontSmall = CreateFontA(
          16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
          DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
      );
      SelectObject(hdc, hFontSmall);
      SetTextColor(hdc, RGB(200, 200, 200));

      std::string statusText = "";
      if (game.mode == GameMode::HOST_WAITING) {
        std::string ip = Network::getHostIP();
        statusText = "Waiting for client... IP: " + ip + ":" + std::to_string(game.networkPort);
      } else if (!game.connectionStatus.empty()) {
        statusText = game.connectionStatus;
      } else {
        statusText = "Use arrow keys to navigate, Enter to select, or click";
      }    
      
      SIZE instSize;
      GetTextExtentPoint32A(hdc, statusText.c_str(), static_cast<int>(statusText.length()), &instSize);
      int instX = (Constants::kWindowWidth - instSize.cx) / 2;
      int instY = Constants::kWindowHeight - 50;
      TextOutA(hdc, instX, instY, statusText.c_str(), static_cast<int>(statusText.length()));

      SelectObject(hdc, hOldFont);
      DeleteObject(hFontSmall);
    }
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFontMenu);
  }

  bool handleClick(int x, int y, GameState& game) {
    int singleplayerY = kMenuStartY;
    int multiplayerY = kMenuStartY + kMenuOptionSpacing;
    int hostY = kMenuStartY + kMenuOptionSpacing * 2;
    int joinY = kMenuStartY + kMenuOptionSpacing * 3;
    int quitY = kMenuStartY + kMenuOptionSpacing * 4;
    int centerX = Constants::kWindowWidth / 2;
    int halfWidth = kMenuOptionWidth / 2;

    if (y >= singleplayerY && y <= singleplayerY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        game.mode = GameMode::PLAYING;
        game.isSinglePlayer = true;
        game.isOnlineMultiplayer = false;
        return true;
      }
    }

    if (y >= multiplayerY && y <= multiplayerY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        game.mode = GameMode::PLAYING;
        game.isSinglePlayer = false;
        game.isOnlineMultiplayer = false;
        return true;
      }
    }

    if (y >= hostY && y <= hostY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        if (Network::startHost(game.networkPort)) {
          game.mode = GameMode::HOST_WAITING;
          game.isOnlineMultiplayer = true;
          game.isSinglePlayer = false;
          game.connectionStatus = "Host started. Waiting for client...";
        } else {
          game.connectionStatus = "Failed to start host. Port may be in use.";
        }
        return true;
      }
    }

    if (y >= joinY && y <= joinY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        game.mode = GameMode::CLIENT_CONNECTING;
        game.hostIPInput = "";
        game.connectionStatus = "Type host IP address";
        return true;
      }
    }

    if (y >= quitY && y <= quitY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        return false;
      }
    }

    return true;
  }

  void handleCharInput(char c, GameState& game) {
    if (game.mode == GameMode::CLIENT_CONNECTING) {
      if (c == '\b' || c == 8) { // Backspace
        if (!game.hostIPInput.empty()) {
          game.hostIPInput.pop_back();
        }
      } else if (c == '\r' || c == '\n') { // Enter
        if (!game.hostIPInput.empty()) {
          if (Network::connectToHost(game.hostIPInput, game.networkPort)) {
            game.mode = GameMode::PLAYING;
            game.isOnlineMultiplayer = true;
            game.isSinglePlayer = false;
            game.connectionStatus = "Connected!";
          } else {
            game.connectionStatus = "Failed to connect to " + game.hostIPInput;
          }
        }
      } else if ((c >= '0' && c <= '9') || c == '.') {
        game.hostIPInput += c;
      }
    }
  }

  void handleKeyDown(WPARAM wParam, GameState& game, HWND hwnd) {
    switch (wParam) {
      case VK_DOWN:
      if (game.selectedMenuOption == MenuOption::SINGLEPLAYER) {
        game.selectedMenuOption = MenuOption::MULTIPLAYER;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::MULTIPLAYER) {
        game.selectedMenuOption = MenuOption::ONLINE_HOST;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::ONLINE_HOST) {
        game.selectedMenuOption = MenuOption::ONLINE_JOIN;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::ONLINE_JOIN) {
        game.selectedMenuOption = MenuOption::QUIT;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::QUIT) {
        game.selectedMenuOption = MenuOption::SINGLEPLAYER;
        InvalidateRect(hwnd, nullptr, FALSE);
      }
      break;
      case VK_UP:
      if (game.selectedMenuOption == MenuOption::SINGLEPLAYER) {
        game.selectedMenuOption = MenuOption::QUIT;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::MULTIPLAYER) {
        game.selectedMenuOption = MenuOption::SINGLEPLAYER;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::ONLINE_HOST) {
        game.selectedMenuOption = MenuOption::MULTIPLAYER;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::ONLINE_JOIN) {
        game.selectedMenuOption = MenuOption::ONLINE_HOST;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::QUIT) {
        game.selectedMenuOption = MenuOption::ONLINE_JOIN;
        InvalidateRect(hwnd, nullptr, FALSE);
      }
      break;
      case VK_RETURN:
        if (game.selectedMenuOption == MenuOption::MULTIPLAYER) {
          game.mode = GameMode::PLAYING;
          game.isSinglePlayer = false;
          game.isOnlineMultiplayer = false;
          InvalidateRect(hwnd, nullptr, FALSE);
        } else if (game.selectedMenuOption == MenuOption::SINGLEPLAYER) {
          game.mode = GameMode::PLAYING;
          game.isSinglePlayer = true;
          game.isOnlineMultiplayer = false;
          InvalidateRect(hwnd, nullptr, FALSE);
        } else if (game.selectedMenuOption == MenuOption::ONLINE_HOST) {
          if (Network::startHost(game.networkPort)) {
            game.mode = GameMode::HOST_WAITING;
            game.isOnlineMultiplayer = true;
            game.isSinglePlayer = false;
            game.connectionStatus = "Host started. Waiting for client...";
          } else {
            game.connectionStatus = "Failed to start host.";
          }
          InvalidateRect(hwnd, nullptr, FALSE);
        } else if (game.selectedMenuOption == MenuOption::ONLINE_JOIN) {
          game.mode = GameMode::CLIENT_CONNECTING;
          game.hostIPInput = "";
          game.connectionStatus = "Type host IP address";
          InvalidateRect(hwnd, nullptr, FALSE);
        } else if (game.selectedMenuOption == MenuOption::QUIT) {
          PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        break;
    }
  }
}

