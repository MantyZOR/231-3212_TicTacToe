#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>

#include "game/game_server.h"
#include "network/network.h"
#include "repository/database_repository.h"


int main(int argc, char *argv[]) {
  try {
	QCoreApplication a(argc, argv);

	GameServer &server = GameServer::getInstance();
	std::thread server_thread([&server] { server.start(); });
	QCoreApplication::exec();
  }
  catch (const std::exception &e) {
	std::cerr << "Error: " << e.what() << std::endl;
	return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
