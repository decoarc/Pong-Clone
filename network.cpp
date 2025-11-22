#include "network.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

static SOCKET hostSocket = INVALID_SOCKET;
static SOCKET clientSocket = INVALID_SOCKET;
static sockaddr_in clientAddress = {};
static int clientAddressLen = sizeof(clientAddress);
static bool clientConnected = false;
static bool clientJustConnected = false;
static NetworkRole currentRole = NetworkRole::NONE;
static bool initialized = false;
static uint32_t messageSequence = 0;
static const int BUFFER_SIZE = 1024;
static char recvBuffer[BUFFER_SIZE];

static const int TIMEOUT_MS = 5000;
static auto lastReceivedTime = std::chrono::steady_clock::now();

bool Network::initialize() {
    if (initialized) return true;

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }

    initialized = true;
    return true;
}

void Network::shutdown() {
    stopHost();
    disconnect();

    if (initialized) {
        WSACleanup();
        initialized = false;
    }
}

bool Network::startHost(int port) {
    if (!initialized && !initialize()) {
        return false;
    }

    if (hostSocket != INVALID_SOCKET) {
        closesocket(hostSocket);
    }

    hostSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (hostSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(hostSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(hostSocket);
        hostSocket = INVALID_SOCKET;
        std::cerr << "Failed to bind socket: " << error << std::endl;
        return false;
    }

    u_long mode = 1;
    if (ioctlsocket(hostSocket, FIONBIO, &mode) == SOCKET_ERROR) {
        closesocket(hostSocket);
        hostSocket = INVALID_SOCKET;
        return false;
    }

    DWORD timeout = TIMEOUT_MS;
    setsockopt(hostSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    currentRole = NetworkRole::HOST;
    clientConnected = false;
    clientJustConnected = false;
    messageSequence = 0;
    lastReceivedTime = std::chrono::steady_clock::now();

    return true;
}

void Network::stopHost() {
    if (hostSocket != INVALID_SOCKET) {
        if (clientConnected) {
            NetworkMessage msg;
            msg.type = NetworkMessageType::DISCONNECT;
            msg.sequence = messageSequence++;
            sendto(hostSocket, (char*)&msg, sizeof(msg), 0, (sockaddr*)&clientAddress, clientAddressLen);
        }

        closesocket(hostSocket);
        hostSocket = INVALID_SOCKET;
    }

    currentRole = NetworkRole::NONE;
    clientConnected = false;
    clientJustConnected = false;
}

bool Network::isHosting() {
    return currentRole == NetworkRole::HOST && hostSocket != INVALID_SOCKET;
}

std::string Network::getHostIP() {
    if (!isHosting()) return "";

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        return "";
    }

    hostent* host = gethostbyname(hostname);
    if (host == nullptr) return "";

    for (int i = 0; host->h_addr_list[i] != nullptr; i++) {
        in_addr addr;
        memcpy(&addr, host->h_addr_list[i], sizeof(addr));
        char* ip = inet_ntoa(addr);
        if (ip && strcmp(ip, "127.0.0.1") != 0) {
            return std::string(ip);
        }
    }

    return "127.0.0.1";
}

void Network::sendGameState(const GameState& game) {
    if (hostSocket == INVALID_SOCKET || !clientConnected) return;

    NetworkMessage msg;
    msg.type = NetworkMessageType::GAME_STATE_UPDATE;
    msg.sequence = messageSequence++;

    msg.leftPaddleY = game.leftPaddleY;
    msg.rightPaddleY = game.rightPaddleY;
    msg.ballX = game.ballX;
    msg.ballY = game.ballY;
    msg.ballDX = game.ballDX;
    msg.ballDY = game.ballDY;
    msg.leftScore = game.leftScore;
    msg.rightScore = game.rightScore;

    int sent = sendto(hostSocket, (char*)&msg, sizeof(msg), 0, (sockaddr*)&clientAddress, clientAddressLen);

    if (sent == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error != WSAEWOULDBLOCK) {
            clientConnected = false;
        }
    }
}

void Network::processIncomingMessages(GameState& game) {
    if (hostSocket == INVALID_SOCKET) return;

    sockaddr_in fromAddress;
    int fromLen = sizeof(fromAddress);

    while (true) {
        int recvLen = recvfrom(hostSocket, recvBuffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddress, &fromLen);

        if (recvLen == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK || error == WSAETIMEDOUT) {
                break;
            }
            break;
        }

        if (recvLen < sizeof(NetworkMessage)) continue;

        NetworkMessage* msg = (NetworkMessage*)recvBuffer;
        lastReceivedTime = std::chrono::steady_clock::now();

        switch (msg->type) {
            case NetworkMessageType::CONNECTION_REQUEST:
            if (!clientConnected) {
                clientAddress = fromAddress;
                clientAddressLen = fromLen;
                clientConnected = true;
                clientJustConnected = true;

                NetworkMessage acceptMsg;
                acceptMsg.type = NetworkMessageType::CONNECTION_ACCEPT;
                acceptMsg.sequence = messageSequence++;
                sendto(hostSocket, (char*)&acceptMsg, sizeof(acceptMsg), 0, (sockaddr*)&fromAddress, fromLen);
            }
            break;

            case NetworkMessageType::CLIENT_INPUT:
            if (clientConnected && fromAddress.sin_addr.s_addr == clientAddress.sin_addr.s_addr && fromAddress.sin_port == clientAddress.sin_port) {
                game.rightPaddleUp = msg->rightPaddleUp;
                game.rightPaddleDown = msg->rightPaddleDown;
            }
            break;

            case NetworkMessageType::PING:
            if (clientConnected) {
                NetworkMessage pong;
                pong.type = NetworkMessageType::PONG;
                pong.sequence = msg->sequence;
                sendto(hostSocket, (char*)&pong, sizeof(pong), 0, (sockaddr*)&fromAddress, fromLen);
            }
            break;

            case NetworkMessageType::DISCONNECT:
            clientConnected = false;
            break;

            default:
            break;
        }
    }
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReceivedTime).count();
    if (clientConnected && elapsed > TIMEOUT_MS) {
        clientConnected = false;
    }
}

bool Network::isClientConnected() {
    if (currentRole != NetworkRole::HOST) return false;
    bool result = clientJustConnected;
    clientJustConnected = false;
    return result;
}



bool Network::connectToHost(const std::string& hostAddress, int port) {
    if (!initialized && !initialize()) {
        return false;
    }

    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }

    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clientSocket == INVALID_SOCKET) {
        return false;
    }

    sockaddr_in hostAddr;
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(port);

    if (inet_pton(AF_INET, hostAddress.c_str(), &hostAddr.sin_addr) != 1) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    if (connect(clientSocket, (sockaddr*)&hostAddr, sizeof(hostAddr)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    u_long mode = 1;
    ioctlsocket(clientSocket, FIONBIO, &mode);

    DWORD timeout = TIMEOUT_MS;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    currentRole = NetworkRole::CLIENT;
    messageSequence = 0;
    lastReceivedTime = std::chrono::steady_clock::now();

    NetworkMessage msg;
    msg.type = NetworkMessageType::CONNECTION_REQUEST;
    msg.sequence = messageSequence++;
    send(clientSocket, (char*)&msg, sizeof(msg), 0);

    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        NetworkMessage response;
        int recvLen = recv(clientSocket, (char*)&response, sizeof(response), 0);

        if (recvLen > 0 && response.type == NetworkMessageType::CONNECTION_ACCEPT) {
            return true;
        }
    }

    closesocket(clientSocket);
    clientSocket = INVALID_SOCKET;
    currentRole = NetworkRole::NONE;
    return false;
}

void Network::disconnect() {
    if (clientSocket != INVALID_SOCKET) {
        NetworkMessage msg;
        msg.type = NetworkMessageType::DISCONNECT;
        msg.sequence = messageSequence++;
        send(clientSocket, (char*)&msg, sizeof(msg), 0);

        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    currentRole = NetworkRole::NONE;
}

bool Network::isConnected() {
    return currentRole == NetworkRole::CLIENT && clientSocket != INVALID_SOCKET;
}

void Network::sendInput(bool up, bool down) {
    if (clientSocket == INVALID_SOCKET) return;

    NetworkMessage msg;
    msg.type = NetworkMessageType::CLIENT_INPUT;
    msg.sequence = messageSequence++;
    msg.rightPaddleUp = up;
    msg.rightPaddleDown = down;

    send(clientSocket, (char*)&msg, sizeof(msg), 0);
}

bool Network::receiveGameState(GameState& game) {
    if (clientSocket == INVALID_SOCKET) return false;

    NetworkMessage msg;
    int recvLen = recv(clientSocket, (char*)&msg, sizeof(msg), 0);

    if (recvLen == SOCKET_ERROR) {
        int error = WSAGetLastError();
        if (error == WSAEWOULDBLOCK || error == WSAETIMEDOUT) {
            return false;
        }

        disconnect();
        return false;
    }

    if (recvLen < sizeof(NetworkMessage)) return false;

    lastReceivedTime = std::chrono::steady_clock::now();

    switch (msg.type) {
        case NetworkMessageType::GAME_STATE_UPDATE:

        game.leftPaddleY = msg.leftPaddleY;
        game.rightPaddleY = msg.rightPaddleY;
        game.ballX = msg.ballX;
        game.ballY = msg.ballY;
        game.ballDX = msg.ballDX;
        game.ballDY = msg.ballDY;
        game.leftScore = msg.leftScore;
        game.rightScore = msg.rightScore;
        return true;

        case NetworkMessageType::PONG:
        return false;

        case NetworkMessageType::DISCONNECT:
        disconnect();
        return false;

        default:
        return false;
    }
}

NetworkRole Network::getRole() {
    return currentRole;
}

bool Network::isOnline() {
    return currentRole != NetworkRole::NONE;
}

void Network::update() {
    static auto lastPingTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPingTime).count();

    if (elapsed > 1000) {
        lastPingTime = now;

        if (isConnected()) {
            NetworkMessage ping;
            ping.type = NetworkMessageType::PING;
            ping.sequence = messageSequence++;
            send(clientSocket, (char*)&ping, sizeof(ping), 0);
        }
    }

    auto timeSinceLastReceived = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastReceivedTime).count();
    
    if (isOnline() && timeSinceLastReceived > TIMEOUT_MS) {
        if (isHosting()) {
            stopHost();
        } else if (isConnected()) {
            disconnect();
        }
    }
}