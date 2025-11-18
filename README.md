# Pong-Clone

Jogo Pong implementado em C++ usando Win32 API.

## Screenshots

![Menu Inicial](screenshots/1.png)

![Jogo em Execução](screenshots/2.png)

## Compilação e Execução

Para compilar o projeto (janela Win32):

```bash
g++ -std=c++11 -o pong.exe main.cpp game_logic.cpp renderer.cpp window.cpp audio.cpp menu.cpp -lgdi32 -luser32 -lwinmm
```

O flag `-mwindows` remove a janela de console e cria uma aplicação Win32.

Para executar:

```bash
.\pong.exe
```
