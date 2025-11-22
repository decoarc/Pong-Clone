#ifndef NETWORK_H
#define NETWORK_H

#include "game_state.h"
#include <string>
#include <cstdint>

enum class NetworkRole {
    NONE,
    HOST,
    CLIENT
};

enum class NetworkMessageType : uint8_t {
    CLIENT_INPUT = 1,
    GAME_STATE_UPDATE = 2,
    CONNECTION_REQUEST = 3,
    CONNECTION_ACCEPT = 4,
    CONNECTION_DENY = 5,
    DISCONNECT = 6,
    PING = 7,
    PONG = 8
};

#pragma pack(push, 1)
struct NetworkMessage {
    NetworkMessageType type;
    uint32_t sequence;

    bool rightPaddleUp;
    bool rightPaddleDown;
    uint8_t padding[2];

    int32_t rightPaddleY;
    int32_t leftPaddleY;
    int32_t ballX;
    int32_t ballY;
    int32_t ballDX;
    int32_t ballDY;
    int32_t leftScore;
    int32_t rightScore;


};
#pragma pack(pop)


namespace Network {
    bool initialize();
    void shutdown();

    bool startHost(int port = 7777);
    void stopHost();
    bool isHosting();
    void sendGameState(const GameState& game);
    void processIncomingMessages(GameState& game);
    std::string getHostIP();
    bool isClientConnected(); // Para host verificar se cliente conectou

    bool connectToHost(const std::string& hostAddress, int port = 7777);
    void disconnect();
    bool isConnected();
    void sendInput(bool up, bool down);
    bool receiveGameState(GameState& game);

    NetworkRole getRole();
    bool isOnline();
    void update();
}

#endif
