#ifndef DATABASE_REPOSITORY_H
#define DATABASE_REPOSITORY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QVariant>
#include <nlohmann/json.hpp>

struct Game {
  int id;
  int playerX;
  int playerO;
  int currentPlayer;
  int winner;
  std::string state;
  std::string roomName;
};

using json = nlohmann::json;

inline void to_json(json &j, const Game &g) {
  j = json{{"id", g.id}, {"playerX", g.playerX}, {"playerO", g.playerO}, {"currentPlayer", g.currentPlayer},
		   {"winner", g.winner}, {"state", g.state}, {"roomName", g.roomName}};
}

inline void from_json(const json &j, Game &g) {
  j.at("id").get_to(g.id);
  j.at("playerX").get_to(g.playerX);
  j.at("playerO").get_to(g.playerO);
  j.at("currentPlayer").get_to(g.currentPlayer);
  j.at("winner").get_to(g.winner);
  j.at("state").get_to(g.state);
  j.at("roomName").get_to(g.roomName);
}

class DatabaseRepository {
 public:
  static DatabaseRepository &getInstance();

  void connectToDatabase();

  void createTables();

  QSqlQuery executeQuery(const QString &query);

  int registerUser(const std::string &username, const std::string &password) const;

  int authenticateUser(const std::string &username, const std::string &password) const;

  std::string getUsernameById(int userId) const;

  Game getGameById(int gameId) const;

  int createGame(int playerID, int playerType);

  void setPlayerX(int gameId, int playerX);

  void setPlayerO(int gameId, int playerO);

  void setCurrentPlayer(int gameId, int currentPlayer);

  void setWinner(int gameId, int winner);

  std::string getState(int gameId);

  void setState(int gameId, const std::string &state);

  int getGameId(const std::string &roomName);

 private:
  DatabaseRepository();

  DatabaseRepository(const DatabaseRepository &) = delete;

  DatabaseRepository &operator=(const DatabaseRepository &) = delete;

  QSqlDatabase db;
  static DatabaseRepository instance;
};

#endif // DATABASE_REPOSITORY_H