#include "game_logic.h"
#include "constants.h"
#include "audio.h"
#include <algorithm>
#include <cstdlib>

namespace GameLogic {
  void clamp(int &value, int minVal, int maxVal) {
    if (value < minVal) value = minVal;
    if (value > maxVal) value = maxVal;
  }

  void resetBall(GameState& game, int towardDX) {
    game.ballX = Constants::kGameWidth / 2;
    game.ballY = Constants::kGameHeight / 2;
    game.ballDX = towardDX;
    game.ballDY = (rand() % 2 == 0) ? -1 : 1;
  }

  void updateGame(GameState& game) {
    if (game.paused) return;

    if (game.leftPaddleUp) game.leftPaddleY -= 1;
    if (game.leftPaddleDown) game.leftPaddleY += 1;
    if (game.rightPaddleUp) game.rightPaddleY -= 1;
    if (game.rightPaddleDown) game.rightPaddleY += 1;

    int minPaddleY = Constants::kTopOffset + 1;
    int maxPaddleY = Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight - 1;
    clamp(game.leftPaddleY, minPaddleY, maxPaddleY);
    clamp(game.rightPaddleY, minPaddleY, maxPaddleY);

    game.ballX += game.ballDX;
    game.ballY += game.ballDY;
    if (game.ballY <= Constants::kTopOffset + 1) {
      game.ballY = Constants::kTopOffset + 1;
      game.ballDY = 1;
      Audio::playSound(Audio::SoundType::WALL_HIT);
    } else if (game.ballY >= Constants::kGameHeight - Constants::kBottomOffset - 2) {
      game.ballY = Constants::kGameHeight - Constants::kBottomOffset - 2;
      game.ballDY = -1;
      Audio::playSound(Audio::SoundType::WALL_HIT);
    }

    int leftPaddleX = Constants::kPaddleMargin;
    if (game.ballDX < 0 && game.ballX >= leftPaddleX && game.ballX <= leftPaddleX + 1) {
      if (game.ballY >= game.leftPaddleY && game.ballY < game.leftPaddleY + Constants::kPaddleHeight) {
        game.ballDX = 1;
        game.ballX = leftPaddleX + 1;
        int hitPos = game.ballY - game.leftPaddleY;
        if (hitPos < Constants::kPaddleHeight / 2) game.ballDY = -1;
        else if (hitPos > Constants::kPaddleHeight / 2) game.ballDY = 1;
        Audio::playSound(Audio::SoundType::PADDLE_HIT);
      }
    }

    int rightPaddleX = Constants::kGameWidth - 1 - Constants::kRightPaddleMargin;
    if (game.ballDX > 0 && game.ballX >= rightPaddleX - 1 && game.ballX <= rightPaddleX) {
      if (game.ballY >= game.rightPaddleY && game.ballY < game.rightPaddleY + Constants::kPaddleHeight) {
        game.ballDX = -1;
        game.ballX = rightPaddleX - 1;
        int hitPos = game.ballY - game.rightPaddleY;
        if (hitPos < Constants::kPaddleHeight / 2) game.ballDY = -1;
        else if (hitPos > Constants::kPaddleHeight / 2) game.ballDY = 1;
        Audio::playSound(Audio::SoundType::PADDLE_HIT);
      }
    }

    if (game.ballX <= 0) {
      game.rightScore += 1;
      resetBall(game, -1);
      Audio::playSound(Audio::SoundType::SCORE);
    } else if (game.ballX >= Constants::kGameWidth - 1) {
      game.leftScore += 1;
      resetBall(game, 1);
      Audio::playSound(Audio::SoundType::SCORE);
    }
  }
}

