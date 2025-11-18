#include <windows.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <algorithm>

// Simple Windows console Pong

namespace {
  const int kScreenWidth = 80;
  const int kScreenHeight = 24;

  const int kPaddleHeight = 5;
  const int kPaddleMargin = 2;
  const int kRightPaddleMargin = 1; // Margem da raquete direita (diminua para afastar do centro)

  const char kBallChar = 'O';
  const char kPaddleChar = '|';
  const char kWallChar = '#';
  const char kCenterLineChar = ':';

  void setCursorVisible(bool visible) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci{};
    cci.dwSize = 25;
    cci.bVisible = visible ? TRUE : FALSE;
    SetConsoleCursorInfo(hOut, &cci);
  }

  void moveCursorToTopLeft() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord{0, 0};
    SetConsoleCursorPosition(hOut, coord);
  }

  void clamp(int &value, int minVal, int maxVal) {
    if (value < minVal) value = minVal;
    if (value > maxVal) value = maxVal;
  }
}

int main() {
  // Resize console buffer to avoid scrolling
  {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SMALL_RECT rect{0, 0, static_cast<SHORT>(kScreenWidth - 1), static_cast<SHORT>(kScreenHeight - 1)};
    COORD size{static_cast<SHORT>(kScreenWidth), static_cast<SHORT>(kScreenHeight)};
    SetConsoleScreenBufferSize(hOut, size);
    SetConsoleWindowInfo(hOut, TRUE, &rect);
  }

  setCursorVisible(false);

  int leftPaddleY = (kScreenHeight - kPaddleHeight) / 2;
  int rightPaddleY = (kScreenHeight - kPaddleHeight) / 2;

  int ballX = kScreenWidth / 2;
  int ballY = kScreenHeight / 2;
  int ballDX = -1; // start moving left
  int ballDY = -1; // start moving up

  int leftScore = 0;
  int rightScore = 0;

  int frameDelayMs = 30; // lower is faster
  bool running = true;
  bool paused = false;

  std::string buffer;
  buffer.resize(static_cast<size_t>(kScreenWidth * kScreenHeight));

  auto resetBall = [&](int towardDX) {
    ballX = kScreenWidth / 2;
    ballY = kScreenHeight / 2;
    ballDX = towardDX;
    ballDY = (rand() % 2 == 0) ? -1 : 1;
  };

  while (running) {
    // INPUT
    if (_kbhit()) {
      int ch = _getch();
      if (ch == 27) { // ESC
        running = false;
      } else if (ch == 'p' || ch == 'P') {
        paused = !paused;
      } else if (ch == 'w' || ch == 'W') {
        leftPaddleY -= 1;
      } else if (ch == 's' || ch == 'S') {
        leftPaddleY += 1;
      } else if (ch == '+') {
        frameDelayMs = std::max(1, frameDelayMs - 1);
      } else if (ch == '-') {
        frameDelayMs = std::min(100, frameDelayMs + 1);
      } else if (ch == 224 || ch == 0) { // arrow keys
        int ch2 = _getch();
        if (ch2 == 72) { // up
          rightPaddleY -= 1;
        } else if (ch2 == 80) { // down
          rightPaddleY += 1;
        }
      }
    }

    clamp(leftPaddleY, 1, kScreenHeight - kPaddleHeight - 1);
    clamp(rightPaddleY, 1, kScreenHeight - kPaddleHeight - 1);

    // UPDATE
    if (!paused) {
      ballX += ballDX;
      ballY += ballDY;

      // Top/bottom wall collisions
      if (ballY <= 1) {
        ballY = 1;
        ballDY = 1;
      } else if (ballY >= kScreenHeight - 2) {
        ballY = kScreenHeight - 2;
        ballDY = -1;
      }

      // Left paddle collision
      int leftPaddleX = kPaddleMargin;
      if (ballDX < 0 && ballX >= leftPaddleX && ballX <= leftPaddleX + 1) {
        if (ballY >= leftPaddleY && ballY < leftPaddleY + kPaddleHeight) {
          ballDX = 1;
          ballX = leftPaddleX + 1; // Garantir posição correta após colisão
          int hitPos = ballY - leftPaddleY; // 0..kPaddleHeight-1
          if (hitPos < kPaddleHeight / 2) ballDY = -1;
          else if (hitPos > kPaddleHeight / 2) ballDY = 1;
        }
      }

      // Right paddle collision
      int rightPaddleX = kScreenWidth - 1 - kRightPaddleMargin;
      if (ballDX > 0 && ballX >= rightPaddleX - 1 && ballX <= rightPaddleX) {
        if (ballY >= rightPaddleY && ballY < rightPaddleY + kPaddleHeight) {
          ballDX = -1;
          ballX = rightPaddleX - 1; // Garantir posição correta após colisão
          int hitPos = ballY - rightPaddleY;
          if (hitPos < kPaddleHeight / 2) ballDY = -1;
          else if (hitPos > kPaddleHeight / 2) ballDY = 1;
        }
      }

      // Scoring (verificar apenas se não houve colisão)
      if (ballX <= 0) {
        rightScore += 1;
        resetBall(-1);
      } else if (ballX >= kScreenWidth - 1) {
        leftScore += 1;
        resetBall(1);
      }
    }

    // RENDER
    // Fill with spaces
    std::fill(buffer.begin(), buffer.end(), ' ');

    auto idx = [&](int x, int y) -> size_t {
      return static_cast<size_t>(y) * kScreenWidth + static_cast<size_t>(x);
    };

    // Walls top/bottom
    for (int x = 0; x < kScreenWidth; ++x) {
      buffer[idx(x, 0)] = kWallChar;
      buffer[idx(x, kScreenHeight - 1)] = kWallChar;
    }

    // Center dashed line
    for (int y = 1; y < kScreenHeight - 1; ++y) {
      if (y % 2 == 0) buffer[idx(kScreenWidth / 2, y)] = kCenterLineChar;
    }

    // Paddles
    for (int i = 0; i < kPaddleHeight; ++i) {
      buffer[idx(kPaddleMargin, leftPaddleY + i)] = kPaddleChar;
      buffer[idx(kScreenWidth - 1 - kRightPaddleMargin, rightPaddleY + i)] = kPaddleChar;
    }

    // Ball
    buffer[idx(ballX, ballY)] = kBallChar;

    // Score and HUD
    {
      std::string hud = "  W/S=Left  Up/Down=Right  P=Pause  +/-=Speed  ESC=Quit  ";
      for (size_t i = 0; i < hud.size() && i < static_cast<size_t>(kScreenWidth - 2); ++i) {
        buffer[idx(1 + static_cast<int>(i), 0)] = hud[i];
      }

      std::string score = "  " + std::to_string(leftScore) + " : " + std::to_string(rightScore) +
                          (paused ? "   [PAUSED]" : "");
      int scoreX = std::max(1, kScreenWidth / 2 - static_cast<int>(score.size()) / 2);
      for (size_t i = 0; i < score.size() && scoreX + static_cast<int>(i) < kScreenWidth - 1; ++i) {
        buffer[idx(scoreX + static_cast<int>(i), kScreenHeight - 1)] = score[i];
      }
    }

    // Blit buffer
    moveCursorToTopLeft();
    for (int y = 0; y < kScreenHeight; ++y) {
      std::cout.write(&buffer[y * kScreenWidth], kScreenWidth);
      if (y < kScreenHeight - 1) std::cout.put('\n');
    }
    std::cout.flush();

    Sleep(static_cast<DWORD>(frameDelayMs));
  }

  setCursorVisible(true);
  moveCursorToTopLeft();
  std::cout << "Thanks for playing!" << std::endl;
  return 0;
}


