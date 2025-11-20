#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "constants.h"

enum class GameMode {
  MENU,
  PLAYING
};

enum class MenuOption {
  MULTIPLAYER,
  SINGLEPLAYER,
  QUIT
};

struct GameState {
  GameMode mode = GameMode::MENU;
  MenuOption selectedMenuOption = MenuOption::SINGLEPLAYER;
  bool isSinglePlayer = false;
  
  int leftPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
  int rightPaddleY = (Constants::kTopOffset + 1 + Constants::kGameHeight - Constants::kBottomOffset - Constants::kPaddleHeight) / 2;
  int ballX = Constants::kGameWidth / 2;
  int ballY = Constants::kGameHeight / 2;
  int ballDX = -1;
  int ballDY = -1;
  int leftScore = 0;
  int rightScore = 0;
  int frameDelayMs = 30;
  bool paused = false;
  bool leftPaddleUp = false;
  bool leftPaddleDown = false;
  bool rightPaddleUp = false;
  bool rightPaddleDown = false;
};

#endif

