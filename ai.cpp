#include "ai.h"
#include "constants.h"
#include <algorithm>
#include <cstdlib>

namespace AI {
  void updatePaddle(GameState& game) {
    int paddleCenterY = game.rightPaddleY + Constants::kPaddleHeight / 2;
    int centerY = (Constants::kTopOffset + 1 + 
                   Constants::kGameHeight - Constants::kBottomOffset - 
                   Constants::kPaddleHeight) / 2;
    
    // Quando a bola está indo para o jogador (esquerda), IA fica no centro
    if (game.ballDX < 0) {
      // Bola indo para longe - volta para o centro da tela
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
      
      // Reset delay counter and current delay when ball is going away
      game.aiReactionDelayCounter = 0;
      game.aiCurrentReactionDelay = 0;
      return;
    }
    
    // Bola indo para direita (em direção ao paddle da IA)
    // Implementar comportamento humano com delay e imprecisão
    
    // Calcular posição alvo da bola
    int targetY = game.ballY;
    
    // Após 5 acertos, adicionar delay de reação e imprecisão para simular erro proposital
    bool shouldAddDelay = (game.aiConsecutiveHits >= 5);
    
    if (shouldAddDelay) {
      // Se o counter foi resetado, calcular novo delay
      if (game.aiReactionDelayCounter == 0) {
        game.aiCurrentReactionDelay = 4 + (rand() % 4); // Delay de 4-7 frames após 5 acertos
      }
      
      // Incrementar contador de delay
      game.aiReactionDelayCounter++;
      
      // Só começa a reagir após o delay
      if (game.aiReactionDelayCounter < game.aiCurrentReactionDelay) {
        game.rightPaddleUp = false;
        game.rightPaddleDown = false;
        return;
      }
      
      // Adicionar imprecisão para simular erro proposital
      int imprecisionRange = 2;
      int imprecision = (rand() % (imprecisionRange * 2 + 1)) - imprecisionRange;
      targetY += imprecision;
    }
    
    // Calcular diferença entre alvo e centro do paddle
    int diff = targetY - paddleCenterY;
    
    // Threshold de reação - após 5 acertos, aumentar threshold para simular reação mais lenta
    // Com threshold 0, IA sempre tenta se alinhar com a bola (mais preciso)
    int reactionThreshold = (game.aiConsecutiveHits >= 5) ? 2 : 0;
    
    // Após 5 acertos, ocasionalmente errar propositalmente
    bool shouldMiss = false;
    if (game.aiConsecutiveHits >= 5) {
      // 25% de chance de errar propositalmente após 5 acertos
      shouldMiss = (rand() % 100) < 25;
    }
    
    if (shouldMiss) {
      // Simular erro: reagir na direção errada ou não reagir a tempo
      int missType = rand() % 3;
      if (missType == 0) {
        // Não reagir - fica parado
        game.rightPaddleUp = false;
        game.rightPaddleDown = false;
      } else if (missType == 1 && abs(diff) > reactionThreshold) {
        // Reagir na direção errada (overshoot)
        if (diff > 0) {
          game.rightPaddleUp = true;
          game.rightPaddleDown = false;
        } else {
          game.rightPaddleUp = false;
          game.rightPaddleDown = true;
        }
      } else {
        // Reagir muito tarde - threshold maior
        if (abs(diff) > reactionThreshold + 2) {
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
    } else if (abs(diff) > reactionThreshold) {
      // Comportamento normal: seguir a bola
      if (diff > 0) {
        // Bola está abaixo do centro do paddle, move para baixo
        game.rightPaddleDown = true;
        game.rightPaddleUp = false;
      } else {
        // Bola está acima do centro do paddle, move para cima
        game.rightPaddleUp = true;
        game.rightPaddleDown = false;
      }
    } else {
      // Próximo o suficiente, para de mover
      game.rightPaddleUp = false;
      game.rightPaddleDown = false;
    }
  }
}

