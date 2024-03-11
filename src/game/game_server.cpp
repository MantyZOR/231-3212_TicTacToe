#include "game_server.h"
#include <iostream>
#include <ostream>
#include "format"

GameServer GameServer::instance;

GameServer& GameServer::getInstance()
{
    return instance;
}

GameServer::GameServer()
    : connectionHandler(handleConnection)
{
}

void GameServer::start() const
{
    Network& network = Network::getInstance();
    network.startListening();
    network.handleConnection(connectionHandler);
}

void GameServer::handleConnection(Socket& client_socket)
{
    std::string message = client_socket.readFromSocket();
    std::cout << "Received: " << message << std::endl;
    client_socket.writeToSocket(std::format("Hello, {}", message) );

    // todo: add json & game logic here
}
