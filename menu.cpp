#include "menu.h"
#include "constants.h"
#include <string>

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

    int singleplayerY = kMenuStartY;
    bool singleplayerSelected = (game.selectedMenuOption == MenuOption::SINGLEPLAYER);
    SetTextColor(hdc, singleplayerSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));
    
    std::string singleplayerText = "SINGLE PLAYER";
    SIZE singleplayerSize;
    GetTextExtentPoint32A(hdc, singleplayerText.c_str(), static_cast<int>(singleplayerText.length()), &singleplayerSize);
    int singleplayerX = (Constants::kWindowWidth - singleplayerSize.cx) / 2;
    TextOutA(hdc, singleplayerX, singleplayerY, singleplayerText.c_str(), static_cast<int>(singleplayerText.length()));

    int multiplayerY = kMenuStartY + kMenuOptionSpacing;
    bool multiplayerSelected = (game.selectedMenuOption == MenuOption::MULTIPLAYER);
    SetTextColor(hdc, multiplayerSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));
    
    std::string multiplayerText = "MULTIPLAYER";
    SIZE multiplayerSize;
    GetTextExtentPoint32A(hdc, multiplayerText.c_str(), static_cast<int>(multiplayerText.length()), &multiplayerSize);
    int multiplayerX = (Constants::kWindowWidth - multiplayerSize.cx) / 2;
    TextOutA(hdc, multiplayerX, multiplayerY, multiplayerText.c_str(), static_cast<int>(multiplayerText.length()));

    int quitY = kMenuStartY + kMenuOptionSpacing * 2;
    bool quitSelected = (game.selectedMenuOption == MenuOption::QUIT);
    SetTextColor(hdc, quitSelected ? RGB(0, 255, 255) : RGB(255, 255, 255));
    
    std::string quitText = "QUIT";
    SIZE quitSize;
    GetTextExtentPoint32A(hdc, quitText.c_str(), static_cast<int>(quitText.length()), &quitSize);
    int quitX = (Constants::kWindowWidth - quitSize.cx) / 2;
    TextOutA(hdc, quitX, quitY, quitText.c_str(), static_cast<int>(quitText.length()));

    HFONT hFontSmall = CreateFontA(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
    );
    SelectObject(hdc, hFontSmall);
    SetTextColor(hdc, RGB(200, 200, 200));
    
    std::string instructions = "Use arrow keys to navigate, Enter to select, or click";
    SIZE instSize;
    GetTextExtentPoint32A(hdc, instructions.c_str(), static_cast<int>(instructions.length()), &instSize);
    int instX = (Constants::kWindowWidth - instSize.cx) / 2;
    int instY = Constants::kWindowHeight - 50;
    TextOutA(hdc, instX, instY, instructions.c_str(), static_cast<int>(instructions.length()));

    SelectObject(hdc, hOldFont);
    DeleteObject(hFontMenu);
    DeleteObject(hFontSmall);
  }

  bool handleClick(int x, int y, GameState& game) {
    int singleplayerY = kMenuStartY;
    int multiplayerY = kMenuStartY + kMenuOptionSpacing;
    int quitY = kMenuStartY + kMenuOptionSpacing * 2;
    int centerX = Constants::kWindowWidth / 2;
    int halfWidth = kMenuOptionWidth / 2;

    if (y >= singleplayerY && y <= singleplayerY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        game.mode = GameMode::PLAYING;
        game.isSinglePlayer = true;
        return true;
      }
    }

    if (y >= multiplayerY && y <= multiplayerY + kMenuOptionHeight) {
      if (x >= centerX - halfWidth && x <= centerX + halfWidth) {
        game.mode = GameMode::PLAYING;
        game.isSinglePlayer = false;
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

  void handleKeyDown(WPARAM wParam, GameState& game, HWND hwnd) {
    switch (wParam) {
      case VK_DOWN:
      if (game.selectedMenuOption == MenuOption::SINGLEPLAYER) {
        game.selectedMenuOption = MenuOption::MULTIPLAYER;
        InvalidateRect(hwnd, nullptr, FALSE);
      } else if (game.selectedMenuOption == MenuOption::MULTIPLAYER) {
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
      } else if (game.selectedMenuOption == MenuOption::QUIT) {
        game.selectedMenuOption = MenuOption::MULTIPLAYER;
        InvalidateRect(hwnd, nullptr, FALSE);
      }
      break;
      case VK_RETURN:
        if (game.selectedMenuOption == MenuOption::MULTIPLAYER) {
          game.mode = GameMode::PLAYING;
          game.isSinglePlayer = false;
          InvalidateRect(hwnd, nullptr, FALSE);
        } else if (game.selectedMenuOption == MenuOption::SINGLEPLAYER) {
          game.mode = GameMode::PLAYING;
          game.isSinglePlayer = true;
          InvalidateRect(hwnd, nullptr, FALSE);
        } else if (game.selectedMenuOption == MenuOption::QUIT) {
          PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        break;
    }
  }
}

