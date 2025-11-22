#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "constants.h"
#include <string>

enum class GameMode {
  MENU,
  PLAYING,
  HOST_WAITING,
  CLIENT_CONNECTING
};

enum class MenuOption {
  MULTIPLAYER,
  SINGLEPLAYER,
  ONLINE_HOST,
  ONLINE_JOIN,
  QUIT
};

struct GameState {
  GameMode mode = GameMode::MENU;
  MenuOption selectedMenuOption = MenuOption::SINGLEPLAYER;
  bool isSinglePlayer = false;
  bool isOnlineMultiplayer = false;

  int networkPort = 7777;
  std::string hostIPInput = "";
  std::string connectionStatus = "";
  
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
  
  // AI state tracking
  int aiConsecutiveHits = 0;
  int aiReactionDelayCounter = 0;
  int aiCurrentReactionDelay = 0; // Delay atual sendo usado
};

#endif

