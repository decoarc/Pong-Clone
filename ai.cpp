#include "ai.h"
#include "constants.h"
#include <algorithm>
#include <cstdlib>

namespace AI {
  void updatePaddle(GameState& game) {
    int paddleCenterY = game.rightPaddleY + Constants::kPaddleHeight / 2;
    int targetY = game.ballY;
    
    // Só move se a bola está vindo em direção ao paddle direito
    if (game.ballDX > 0) {  // Bola indo para direita (em direção ao paddle da IA)
      int diff = targetY - paddleCenterY;
      
      // Adiciona um pouco de imperfeição para não ser perfeito
      // Ajuste este valor para controlar a dificuldade (maior = mais imperfeito)
      int reactionThreshold = 1;
      
      if (abs(diff) > reactionThreshold) {
        if (diff > 0) {
          // Bola está abaixo do centro do paddle, move para baixo
          game.rightPaddleDown = true;
          game.rightPaddleUp = false;
        } else if (diff < 0) {
          // Bola está acima do centro do paddle, move para cima
          game.rightPaddleUp = true;
          game.rightPaddleDown = false;
        }
      } else {
        // Próximo o suficiente, para de mover
        game.rightPaddleUp = false;
        game.rightPaddleDown = false;
      }
    } else {
      // Bola indo para longe - volta para o centro da tela
      int centerY = (Constants::kTopOffset + 1 + 
                     Constants::kGameHeight - Constants::kBottomOffset - 
                     Constants::kPaddleHeight) / 2;
      int diff = centerY - paddleCenterY;
      
      if (abs(diff) > 1) {
        if (diff > 0) {
          game.rightPaddleDown = true;
          game.rightPaddleUp = false;
        } else {
          game.rightPaddleUp = true;
          game.rightPaddleDown = false;
        }
      } else {
        game.rightPaddleUp = false;
        game.rightPaddleDown = false;
      }
    }
  }
}

