# Pong-Clone

Jogo Pong implementado em C++ usando Win32 API.

## Compilação e Execução

Para compilar o projeto (janela Win32):

```bash
g++ -std=c++11 -o pong.exe main.cpp game_logic.cpp renderer.cpp window.cpp -lgdi32 -luser32
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
