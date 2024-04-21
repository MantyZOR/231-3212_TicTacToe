#include "game_server.h"
#include <iostream>
#include <ostream>
#include "format"
#include "../repository/database_repository.h"
#include <nlohmann/json.hpp>

/**
 * @brief Singleton instance of GameServer
 */
GameServer GameServer::instance;

/**
 * @brief Returns the singleton instance of GameServer
 * @return GameServer& - reference to the singleton instance
 */
GameServer &GameServer::getInstance() {
  return instance;
}

/**
 * @brief Constructor for GameServer
 */
GameServer::GameServer()
	: connectionHandler(handleConnection) {
}

/**
 * @brief Starts the game server
 */
void GameServer::start() const {
  auto &repository = DatabaseRepository::getInstance();
  repository.connectToDatabase();
  repository.createTables();

  auto &network = Network::getInstance();
  network.startListening();
  network.handleConnection(connectionHandler);
}

/**
 * @brief Enum class for status codes
 */
enum class STATUS_CODES {
  REGISTER = 0,
  AUTHENTICATE = 1,
  CREATE_ROOM = 2,
  JOIN_ROOM = 3,
  LEAVE_ROOM = 4,
  GET_ROOMS = 5,
  ACTION = 6,
  GET_GAME = 7,
};

using json = nlohmann::json;

/**
 * @brief Converts a string to a 2D vector state
 * @param stateStr - the string to convert
 * @return std::vector<std::vector<int>> - the converted state
 */
std::vector<std::vector<int>> stringToState(const std::string &stateStr) {
  std::vector state(3, std::vector(3, 0));
  for (int i = 0; i < 3; ++i) {
	for (int j = 0; j < 3; ++j) {
	  state[i][j] = stateStr[i * 3 + j] - '0';
	}
  }
  return state;
}

/**
 * @brief Checks if there is a winner in the game
 * @param state - the game state to check
 * @return bool - true if there is a winner, false otherwise
 */
bool check_winner(const std::string &state) {
  auto buttons = stringToState(state);
  // Check rows
  for (int i = 0; i < 3; ++i) {
	if (buttons[i][0] != 0 &&
		buttons[i][0] == buttons[i][1] &&
		buttons[i][0] == buttons[i][2]) {
	  return true;
	}
  }

  // Check columns
  for (int i = 0; i < 3; ++i) {
	if (buttons[0][i] != 0 &&
		buttons[0][i] == buttons[1][i] &&
		buttons[0][i] == buttons[2][i]) {
	  return true;
	}
  }

  // Check diagonals
  if (buttons[0][0] != 0 &&
	  buttons[0][0] == buttons[1][1] &&
	  buttons[0][0] == buttons[2][2]) {
	return true;
  }

  if (buttons[0][2] != 0 &&
	  buttons[0][2] == buttons[1][1] &&
	  buttons[0][2] == buttons[2][0]) {
	return true;
  }

  return false;
}

/**
 * @brief Handles the REGISTER case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleRegister(const json &message, Socket &client_socket) {
  std::string username = message["login"];
  std::string password = message["password"];
  json result;
  result["type"] = STATUS_CODES::REGISTER;
  result["result"] = DatabaseRepository::getInstance().registerUser(username, password);
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the AUTHENTICATE case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleAuthenticate(const json &message, Socket &client_socket) {
  std::string username = message["login"];
  std::string password = message["password"];
  json result;
  result["type"] = STATUS_CODES::AUTHENTICATE;
  result["result"] = DatabaseRepository::getInstance().authenticateUser(username, password);
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the JOIN_ROOM case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleJoinRoom(const json &message, Socket &client_socket) {
  int playerID = message["playerID"];
  int gameID = message["gameID"];

  // check if playerX filled then set playerID to playerO else set playerID to playerX and set currentPlayer to playerX
  // if both playerX and playerO are filled then return error
  QSqlQuery query;
  query.prepare("SELECT playerX, playerO FROM Games WHERE id = :gameId");
  query.bindValue(":gameId", gameID);
  query.exec();

  if (query.next()) {
	auto playerX = query.value(0).toInt();
	auto playerO = query.value(1).toInt();

	if (playerX == -1) {
	  query.prepare("UPDATE Games SET playerX = :playerID WHERE id = :gameId");
	  query.bindValue(":playerID", playerID);
	  query.bindValue(":gameId", gameID);
	  query.exec();
	} else if (playerO == -1) {
	  query.prepare("UPDATE Games SET playerO = :playerID WHERE id = :gameId");
	  query.bindValue(":playerID", playerID);
	  query.bindValue(":gameId", gameID);
	  query.exec();
	} else {
	  json result;
	  result["type"] = STATUS_CODES::JOIN_ROOM;
	  result["result"] = -1;
	  client_socket.writeToSocket(result.dump());
	  std::cout << "Both players are already filled" << std::endl;
	  return;
	}
  }

  query.prepare("UPDATE Games SET currentPlayer = :playerX WHERE id = :gameId");
  query.bindValue(":playerX", playerID);
  query.bindValue(":gameId", gameID);
  query.exec();

  json result;
  result["type"] = STATUS_CODES::JOIN_ROOM;
  result["result"] = DatabaseRepository::getInstance().getGameById(gameID);
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the CREATE_ROOM case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleCreateRoom(const json &message, Socket &client_socket) {
  int playerID = message["playerID"];
  int playerType = message["playerType"];

  json result;
  result["type"] = STATUS_CODES::CREATE_ROOM;
  auto gameID = DatabaseRepository::getInstance().createGame(playerID, playerType);
  result["result"] = DatabaseRepository::getInstance().getGameById(gameID);
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the LEAVE_ROOM case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleLeaveRoom(const json &message, Socket &client_socket) {
  auto playerID = message["playerId"].get<int>();
  auto gameId = message["gameId"].get<int>();

  QSqlQuery query;
  query.prepare("SELECT playerX, playerO, winner FROM Games WHERE id = :gameId");
  query.bindValue(":gameId", gameId);
  query.exec();

  if (query.next()) {
	auto playerX = query.value(0).toInt();
	auto playerO = query.value(1).toInt();
	auto winner = query.value(2).toInt();

	if (winner != -1) {
	  json result;
	  result["type"] = STATUS_CODES::LEAVE_ROOM;
	  client_socket.writeToSocket(result.dump());
	  return;
	}


	if (playerX == playerID) {
	  query.prepare("UPDATE Games SET playerX = -1 WHERE id = :gameId");
	  query.bindValue(":gameId", gameId);
	  query.exec();
	} else if (playerO == playerID) {
	  query.prepare("UPDATE Games SET playerO = -1 WHERE id = :gameId");
	  query.bindValue(":gameId", gameId);
	  query.exec();
	}
  }

  query.prepare("SELECT playerX, playerO FROM Games WHERE id = :gameId");
  query.bindValue(":gameId", gameId);
  query.exec();
  if (query.next()) {
	auto playerX = query.value(0).toInt();
	auto playerO = query.value(1).toInt();
	if (playerX == -1 && playerO == -1) {
	  query.prepare("DELETE FROM Games WHERE id = :gameId");
	  query.bindValue(":gameId", gameId);
	  query.exec();
	}
  }

  json result;
  result["type"] = STATUS_CODES::LEAVE_ROOM;
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the GET_ROOMS case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleGetRooms(const json &message, Socket &client_socket) {
  QSqlQuery query;
  query.prepare("SELECT id FROM Games WHERE playerX == -1 OR playerO == -1");
  query.exec();

  json games = json::array();
  while (query.next()) {
	games.emplace_back(DatabaseRepository::getInstance().getGameById(query.value(0).toInt()));
  }

  json result;
  result["type"] = STATUS_CODES::GET_ROOMS;
  result["result"] = games;
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the GET_GAME case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleGetGame(const json &message, Socket &client_socket) {
  int gameID = message["gameID"];
  json result;
  result["type"] = STATUS_CODES::GET_GAME;
  result["result"] = DatabaseRepository::getInstance().getGameById(gameID);
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles the ACTION case
 * @param message The JSON message received
 * @param client_socket The client socket
 */
void handleAction(const json &message, Socket &client_socket) {
  int gameId = message["gameId"];
  int playerId = message["playerId"];
  int cell = message["cell"];

  QSqlQuery query;
  query.prepare("SELECT state, playerX, playerO, winner FROM Games WHERE id = :gameId");
  query.bindValue(":gameId", gameId);
  query.exec();

  int player_type = -1;

  if (query.next()) {
	auto state = query.value(0).toString().toStdString();
	auto playerX = query.value(1).toInt();
	auto playerO = query.value(2).toInt();
	auto winner = query.value(3).toInt();

	if (winner != -1) {
	  json result;
	  result["type"] = STATUS_CODES::ACTION;
	  result["result"] = DatabaseRepository::getInstance().getGameById(gameId);
	  client_socket.writeToSocket(result.dump());
	  return;
	}

	if (state[cell] == '0') {
	  state[cell] = playerId == playerX ? '1' : '2';
	  query.prepare("UPDATE Games SET state = :state WHERE id = :gameId");
	  query.bindValue(":state", QString::fromStdString(state));
	  query.bindValue(":gameId", gameId);
	  query.exec();
	}

	if (check_winner(state)) {
	  query.prepare("SELECT currentPlayer FROM Games WHERE id = :gameId");
	  query.bindValue(":gameId", gameId);
	  query.exec();

	  if (query.next()) {
		auto currentPlayer = query.value(0).toInt();

		query.prepare("UPDATE Games SET winner = :winner WHERE id = :gameId");
		query.bindValue(":winner", currentPlayer == playerX ? playerX : playerO);
		query.bindValue(":gameId", gameId);
		query.exec();
	  }
	}
  }

  query.prepare("SELECT currentPlayer, playerX, playerO FROM Games WHERE id = :gameId");
  query.bindValue(":gameId", gameId);
  query.exec();

  if (query.next()) {
	auto currentPlayer = query.value(0).toInt();
	auto playerX = query.value(1).toInt();
	auto playerO = query.value(2).toInt();

	if (currentPlayer == playerX) {
	  query.prepare("UPDATE Games SET currentPlayer = :playerO WHERE id = :gameId");
	  query.bindValue(":playerO", playerO);
	  query.bindValue(":gameId", gameId);
	  query.exec();
	} else {
	  query.prepare("UPDATE Games SET currentPlayer = :playerX WHERE id = :gameId");
	  query.bindValue(":playerX", playerX);
	  query.bindValue(":gameId", gameId);
	  query.exec();
	}
  }

  json result;
  result["type"] = STATUS_CODES::ACTION;
  result["result"] = DatabaseRepository::getInstance().getGameById(gameId);
  client_socket.writeToSocket(result.dump());
}

/**
 * @brief Handles a connection from a client
 * @param client_socket - the client socket to handle
 */
void GameServer::handleConnection(Socket &client_socket) {
  auto data = client_socket.readFromSocket();
  try {
	if (data.empty()) {
	  std::cout << "Received empty data" << std::endl;
	  return;
	}

	std::cout << "Received: " << data << std::endl;
	json message = json::parse(data);

	switch (static_cast<STATUS_CODES>(message["type"].get<int>())) {
	  case STATUS_CODES::REGISTER: handleRegister(message, client_socket);
		break;
	  case STATUS_CODES::AUTHENTICATE: handleAuthenticate(message, client_socket);
		break;
	  case STATUS_CODES::JOIN_ROOM: handleJoinRoom(message, client_socket);
		break;
	  case STATUS_CODES::CREATE_ROOM: handleCreateRoom(message, client_socket);
		break;
	  case STATUS_CODES::LEAVE_ROOM: handleLeaveRoom(message, client_socket);
		break;
	  case STATUS_CODES::GET_ROOMS: handleGetRooms(message, client_socket);
		break;
	  case STATUS_CODES::GET_GAME: handleGetGame(message, client_socket);
		break;
	  case STATUS_CODES::ACTION: handleAction(message, client_socket);
		break;
	}
  }
  catch (const json::type_error &e) {
	std::cerr << "JSON type error: " << e.what() << '\n';
	std::cerr << "JSON data: " << data << '\n';
  }
  catch (const json::exception &e) {
	std::cerr << "JSON exception: " << e.what() << '\n';
	std::cerr << "JSON data: " << data << '\n';
  }
  catch (const std::exception &e) {
	std::cerr << "An error occurred: " << e.what() << '\n';
	std::cerr << "JSON data: " << data << '\n';
  }
}
