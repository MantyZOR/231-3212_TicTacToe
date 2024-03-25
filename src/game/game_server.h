#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "../network/network.h"

class GameServer
{
public:
    static GameServer& getInstance();
    void               start() const;

    void startGame();
    void createRoom(const std::string& roomName);
    void joinRoom(const std::string& roomName);
    void leaveGame();

  private:
    GameServer();
    GameServer(const GameServer&)            = delete;
    GameServer& operator=(const GameServer&) = delete;

    static void handleConnection(Socket& client_socket);

    Network::Callback connectionHandler;
    static GameServer instance;
};


#endif //GAME_SERVER_H
