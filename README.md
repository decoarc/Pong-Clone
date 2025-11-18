# Pong-Clone

Jogo Pong implementado em C++ usando Win32 API.

## Compilação e Execução

Para compilar o projeto (janela Win32):

```bash
g++ -std=c++11 -o pong.exe main.cpp game_logic.cpp renderer.cpp window.cpp audio.cpp -lgdi32 -luser32 -lwinmm
```

O flag `-mwindows` remove a janela de console e cria uma aplicação Win32.

Para executar:

```bash
.\pong.exe
```

## Controles

- **W/S**: Mover raquete esquerda para cima/baixo
- **Seta para cima/baixo**: Mover raquete direita para cima/baixo
- **P**: Pausar/despausar o jogo
- **+/-**: Aumentar/diminuir velocidade
- **ESC**: Sair do jogo

## Sons

O jogo possui sistema de som que toca efeitos sonoros para:

- **Colisão com raquete**: Som ao rebater a bola
- **Colisão com parede**: Som ao bater nas paredes superior/inferior
- **Pontuação**: Som quando um jogador marca ponto

O sistema tenta carregar arquivos WAV personalizados da pasta `sounds/`:

- `sounds/paddle_hit.wav` - Som de colisão com raquete
- `sounds/wall_hit.wav` - Som de colisão com parede
- `sounds/score.wav` - Som de pontuação

Se os arquivos não forem encontrados, o jogo usa sons do sistema Windows como fallback.
