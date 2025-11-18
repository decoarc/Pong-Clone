#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>

namespace Audio {
  enum class SoundType {
    PADDLE_HIT,    // Colisão com raquete
    WALL_HIT,      // Colisão com parede
    SCORE          // Pontuação
  };

  void playSound(SoundType type);
  void initialize();
  void cleanup();
}

#endif

