#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game_state.h"

namespace GameLogic {
  void resetBall(GameState& game, int towardDX);
  void updateGame(GameState& game);
}

#endif

