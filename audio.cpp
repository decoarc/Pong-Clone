#include "audio.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace Audio {

  void playSound(SoundType type) {
    const char* soundFile = nullptr;
    
    switch (type) {
      case SoundType::PADDLE_HIT:
        soundFile = "sounds/paddle_hit.wav";
        break;
      case SoundType::WALL_HIT:
        soundFile = "sounds/wall_hit.wav";
        break;
      case SoundType::SCORE:
        soundFile = "sounds/score.wav";
        break;
    }
    
    if (soundFile) {
      // Tenta tocar arquivo WAV, se não encontrar usa som do sistema como fallback
      if (PlaySoundA(soundFile, nullptr, SND_FILENAME | SND_ASYNC | SND_NODEFAULT) == FALSE) {
        // Fallback para sons do sistema se arquivo não existir
        UINT systemSound = 0;
        switch (type) {
          case SoundType::PADDLE_HIT:
            systemSound = MB_ICONASTERISK;
            break;
          case SoundType::WALL_HIT:
            systemSound = MB_ICONEXCLAMATION;
            break;
          case SoundType::SCORE:
            systemSound = MB_ICONQUESTION;
            break;
        }
        if (systemSound != 0) {
          MessageBeep(systemSound);
        }
      }
    }
  }

  void initialize() {
    // Inicialização se necessário no futuro
  }

  void cleanup() {
    // Cleanup se necessário no futuro
  }
}

