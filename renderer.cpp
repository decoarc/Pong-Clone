#include "renderer.h"
#include "constants.h"
#include "menu.h"
#include <string>

namespace Renderer {
  void drawGame(HDC hdc, HWND hwnd, const GameState& game) {
    // Fill background (black)
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

    // Create white brush for drawing
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

    // Draw walls (top/bottom)
    for (int x = 0; x < Constants::kGameWidth ; ++x) {
      drawCell(x, Constants::kTopOffset);
      drawCell(x, Constants::kGameHeight - Constants::kTopOffset - 1);
    }

    // Draw center line
    for (int y = Constants::kTopOffset + 1; y < Constants::kGameHeight - Constants::kBottomOffset - 1; ++y) {
      if (y % 2 == 0) {
        drawCell(Constants::kGameWidth / 2, y);
      }
    }

    // Draw paddles
    for (int i = 0; i < Constants::kPaddleHeight; ++i) {
      drawCell(Constants::kPaddleMargin, game.leftPaddleY + i);
      drawCell(Constants::kGameWidth - 1 - Constants::kRightPaddleMargin, game.rightPaddleY + i);
    }

    // Draw ball
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

    // Draw score and HUD
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    
    // Criar fonte maior para o placar
    HFONT hFont = CreateFontA(
        48,                          // altura
        0,                           // largura
        0,                           // ângulo
        0,                           // orientação
        FW_BOLD,                     // peso (bold)
        FALSE,                       // itálico
        FALSE,                       // sublinhado
        FALSE,                       // riscado
        DEFAULT_CHARSET,            // charset
        OUT_DEFAULT_PRECIS,         // precisão de saída
        CLIP_DEFAULT_PRECIS,         // precisão de clipping
        DEFAULT_QUALITY,             // qualidade
        DEFAULT_PITCH | FF_DONTCARE, // pitch e família
        "Arial"                      // nome da fonte
    );
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    // Desenhar placar no topo centralizado
    std::string score = std::to_string(game.leftScore) + " : " + std::to_string(game.rightScore);
    
    // Calcular tamanho do texto para centralizar
    SIZE textSize;
    GetTextExtentPoint32A(hdc, score.c_str(), static_cast<int>(score.length()), &textSize);
    int scoreX = (Constants::kWindowWidth - textSize.cx) / 2;
    int scoreY = Constants::kTopOffset * Constants::kCellHeight - textSize.cy - 10; // Posição no topo
    
    TextOutA(hdc, scoreX, scoreY, score.c_str(), static_cast<int>(score.length()));
    
    // Restaurar fonte antiga e deletar a nova
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    
    // HUD com fonte menor
    HFONT hFontSmall = CreateFontA(
        16,                          // altura menor
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
    );
    SelectObject(hdc, hFontSmall);
    
    std::string hud = "  W/S=Left  Up/Down=Right  P=Pause  +/-=Speed  ESC=Quit  ";
    TextOutA(hdc, 10, 5, hud.c_str(), static_cast<int>(hud.length()));
    
    if (game.paused) {
        std::string paused = "[PAUSED]";
        SIZE pausedSize;
        GetTextExtentPoint32A(hdc, paused.c_str(), static_cast<int>(paused.length()), &pausedSize);
        int pausedX = (Constants::kWindowWidth - pausedSize.cx) / 2;
        TextOutA(hdc, pausedX, scoreY + 60, paused.c_str(), static_cast<int>(paused.length()));
    }
    
    // Cleanup
    SelectObject(hdc, hOldFont);
    DeleteObject(hFontSmall);

    // Cleanup
    SelectObject(hdc, oldBrush);
    DeleteObject(whiteBrush);
  }

  void draw(HDC hdc, HWND hwnd, const GameState& game) {
    if (game.mode == GameMode::MENU) {
      Menu::drawMenu(hdc, hwnd, game);
    } else {
      drawGame(hdc, hwnd, game);
    }
  }
}

