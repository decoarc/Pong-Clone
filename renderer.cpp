#include "renderer.h"
#include "constants.h"
#include "menu.h"
#include <string>

namespace Renderer {
  void drawGame(HDC hdc, HWND hwnd, const GameState& game) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, whiteBrush);

    auto drawCell = [&](int x, int y) {
      RECT cellRect;
      cellRect.left = x * Constants::kCellWidth;
      cellRect.top = y * Constants::kCellHeight;
      cellRect.right = (x + 1) * Constants::kCellWidth;
      cellRect.bottom = (y + 1) * Constants::kCellHeight;
      FillRect(hdc, &cellRect, whiteBrush);
    };

    for (int x = 0; x < Constants::kGameWidth ; ++x) {
      drawCell(x, Constants::kTopOffset);
      drawCell(x, Constants::kGameHeight - Constants::kTopOffset - 1);
    }
    for (int y = Constants::kTopOffset + 1; y < Constants::kGameHeight - Constants::kBottomOffset - 1; ++y) {
      if (y % 2 == 0) {
        drawCell(Constants::kGameWidth / 2, y);
      }
    }

    for (int i = 0; i < Constants::kPaddleHeight; ++i) {
      drawCell(Constants::kPaddleMargin, game.leftPaddleY + i);
      drawCell(Constants::kGameWidth - 1 - Constants::kRightPaddleMargin, game.rightPaddleY + i);
    }
    int ballCenterX = game.ballX * Constants::kCellWidth + Constants::kCellWidth / 2;
    int ballCenterY = game.ballY * Constants::kCellHeight + Constants::kCellHeight / 2;
    int ballRadius = Constants::kBallSize / 2;
    RECT ballRect = {
      ballCenterX - ballRadius,
      ballCenterY - ballRadius,
      ballCenterX + ballRadius,
      ballCenterY + ballRadius
    };
    FillRect(hdc, &ballRect, whiteBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    HFONT hFont = CreateFontA(
        48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    std::string score = std::to_string(game.leftScore) + " : " + std::to_string(game.rightScore);
    
    SIZE textSize;
    GetTextExtentPoint32A(hdc, score.c_str(), static_cast<int>(score.length()), &textSize);
    int scoreX = (Constants::kWindowWidth - textSize.cx) / 2;
    int scoreY = Constants::kTopOffset * Constants::kCellHeight - textSize.cy - 10;
    
    TextOutA(hdc, scoreX, scoreY, score.c_str(), static_cast<int>(score.length()));
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    
    HFONT hFontSmall = CreateFontA(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
    );
    SelectObject(hdc, hFontSmall);
    
    std::string hud = game.isSinglePlayer 
      ? "  W/S=Move  P=Pause  +/-=Speed  ESC=Quit  "
      : "  W/S=Left  Up/Down=Right  P=Pause  +/-=Speed  ESC=Quit  ";
    TextOutA(hdc, 10, 5, hud.c_str(), static_cast<int>(hud.length()));
    
    if (game.paused) {
        std::string paused = "[PAUSED]";
        SIZE pausedSize;
        GetTextExtentPoint32A(hdc, paused.c_str(), static_cast<int>(paused.length()), &pausedSize);
        int pausedX = (Constants::kWindowWidth - pausedSize.cx) / 2;
        TextOutA(hdc, pausedX, scoreY + 60, paused.c_str(), static_cast<int>(paused.length()));
    }
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hFontSmall);

    SelectObject(hdc, oldBrush);
    DeleteObject(whiteBrush);
  }

  void draw(HDC hdc, HWND hwnd, const GameState& game) {
    if (game.mode == GameMode::MENU || game.mode == GameMode::HOST_WAITING || game.mode == GameMode::CLIENT_CONNECTING) {
      Menu::drawMenu(hdc, hwnd, game);
    } else {
      drawGame(hdc, hwnd, game);
    }
  }
}

