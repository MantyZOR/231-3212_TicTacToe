#include "database_repository.h"

#include <iostream>
#include <QDebug>
#include <QCryptographicHash>

DatabaseRepository DatabaseRepository::instance;

DatabaseRepository& DatabaseRepository::getInstance()
{
    return instance;
}

DatabaseRepository::DatabaseRepository()
{
}

void DatabaseRepository::connectToDatabase()
{
    if (!QSqlDatabase::isDriverAvailable("QSQLITE"))
    {
        std::cout << "Error: SQLite driver is not available" << std::endl;
        return;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/easton/Documents/tictactoe_server/test.db");

    if (!db.open())
    {
        std::cout << "Error: connection with database failed" << std::endl;
    }
}

void DatabaseRepository::createTables()
{
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS Users("
             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
             "username VARCHAR(20) NOT NULL, "
             "password_hash VARCHAR(64) NOT NULL"
             ")");

    query.exec("CREATE TABLE IF NOT EXISTS Games ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
           "playerX INTEGER, "
           "playerO INTEGER, "
           "currentPlayer INTEGER, "
           "winner INTEGER, "
           "state CHAR(9), "
           "roomName VARCHAR(64) NOT NULL,"
           "FOREIGN KEY(playerX) REFERENCES Users(id), "
           "FOREIGN KEY(playerO) REFERENCES Users(id), "
           "FOREIGN KEY(currentPlayer) REFERENCES Users(id), "
           "FOREIGN KEY(winner) REFERENCES Users(id)"
           ")");
}

QSqlQuery DatabaseRepository::executeQuery(const QString& query)
{
    QSqlQuery sqlQuery(db);
    sqlQuery.exec(query);
    return sqlQuery;
}

int DatabaseRepository::registerUser(const std::string& username, const std::string& password) const
{
    if (username.empty() || password.empty())
        return -1;

    // Hash the password
    QByteArray passwordData(password.c_str(), password.size());
    QByteArray passwordHash = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256).toHex();

    // Check if the user already exists in the database
    QSqlQuery query_check;
    query_check.prepare("SELECT * FROM Users WHERE username = :username");
    query_check.bindValue(":username", QString::fromStdString(username));
    if (query_check.exec() && query_check.next())
        return -1;

    // Insert the user into the Users table
    QSqlQuery query;
    query.prepare("INSERT INTO Users(username, password_hash) VALUES (:username, :password_hash)");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":password_hash", QString::fromStdString(std::string(passwordHash.constData(), passwordHash.size())));
    if (query.exec())
        return query.lastInsertId().toInt();

    return -1;
}

int DatabaseRepository::authenticateUser(const std::string& username, const std::string& password) const
{
    // Hash the password
    QByteArray passwordData(password.c_str(), password.size());
    QByteArray passwordHash = QCryptographicHash::hash(passwordData, QCryptographicHash::Sha256).toHex();

    // Check if the user exists in the Users table with the given username and password hash
    QSqlQuery query;
    query.prepare("SELECT * FROM Users WHERE username = :username AND password_hash = :password_hash");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":password_hash", QString::fromStdString(std::string(passwordHash.constData(), passwordHash.size())));
    if (query.exec() && query.next())
    {
        return query.value("id").toInt();
    }

    return -1;
}

Game DatabaseRepository::getGameById(int gameId) const
{
    QSqlQuery query;
    query.prepare("SELECT playerX, playerO, currentPlayer, winner, state, roomName FROM Games WHERE id = :gameId");
    query.bindValue(":gameId", gameId);
    query.exec();
    if (query.next())
    {
        Game game;
        game.id = gameId;
        game.playerX = query.value(0).toInt();
        game.playerO = query.value(1).toInt();
        game.currentPlayer = query.value(2).toInt();
        game.winner = query.value(3).toInt();
        game.state = query.value(4).toString().toStdString();
        game.roomName = query.value(5).toString().toStdString();
        return game;
    }
    return Game{};
}

std::string DatabaseRepository::getUsernameById(int userId) const
{
    QSqlQuery query;
    query.prepare("SELECT username FROM Users WHERE id = :userId");
    query.bindValue(":userId", userId);
    query.exec();
    if (query.next())
    {
        return query.value(0).toString().toStdString();
    }
    return "";
}


int DatabaseRepository::createGame(int playerID, int playerType)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Games(playerX, playerO, currentPlayer, winner, state, roomName) VALUES (:playerX, :playerO, :currentPlayer, :winner, :state, :roomName)");

    query.bindValue(":playerX", playerType == 1 ? playerID : -1);
    query.bindValue(":playerO", playerType != 1 ? playerID : -1);
    query.bindValue(":currentPlayer", playerType == 1 ? playerID : -1);
    query.bindValue(":winner", -1);
    query.bindValue(":state", "000000000");
    query.bindValue(":roomName", "");
    query.exec();

    const auto gameId = query.lastInsertId().toInt();

    query.prepare("UPDATE Games SET roomName = :roomName WHERE id = :gameId");
    query.bindValue(":roomName", std::format("{} Game #{}", getUsernameById(playerID), gameId).c_str());
    query.bindValue(":gameId", gameId);
    query.exec();

    return gameId;
}

void DatabaseRepository::setPlayerX(int gameId, int playerX)
{
    QSqlQuery query;
    query.prepare("UPDATE Games SET playerX = :playerX WHERE id = :gameId");
    query.bindValue(":playerX", playerX);
    query.bindValue(":gameId", gameId);
    query.exec();
}

void DatabaseRepository::setPlayerO(int gameId, int playerO)
{
    QSqlQuery query;
    query.prepare("UPDATE Games SET playerO = :playerO WHERE id = :gameId");
    query.bindValue(":playerO", playerO);
    query.bindValue(":gameId", gameId);
    query.exec();
}

void DatabaseRepository::setCurrentPlayer(int gameId, int currentPlayer)
{
    QSqlQuery query;
    query.prepare("UPDATE Games SET currentPlayer = :currentPlayer WHERE id = :gameId");
    query.bindValue(":currentPlayer", currentPlayer);
    query.bindValue(":gameId", gameId);
    query.exec();
}

void DatabaseRepository::setWinner(int gameId, int winner)
{
    QSqlQuery query;
    query.prepare("UPDATE Games SET winner = :winner WHERE id = :gameId");
    query.bindValue(":winner", winner);
    query.bindValue(":gameId", gameId);
    query.exec();
}

std::string DatabaseRepository::getState(int gameId)
{
    QSqlQuery query;
    query.prepare("SELECT state FROM Games WHERE id = :gameId");
    query.bindValue(":gameId", gameId);
    query.exec();
    if (query.next())
    {
        return query.value(0).toString().toStdString();
    }
    return "";
}

void DatabaseRepository::setState(int gameId, const std::string& state)
{
    QSqlQuery query;
    query.prepare("UPDATE Games SET state = :state WHERE id = :gameId");
    query.bindValue(":state", QString::fromStdString(state));
    query.bindValue(":gameId", gameId);
    query.exec();
}

int DatabaseRepository::getGameId(const std::string& roomName)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM Games WHERE roomName = :roomName");
    query.bindValue(":roomName", QString::fromStdString(roomName));
    query.exec();
    if (query.next())
    {
        return query.value(0).toInt();
    }
    return -1;
}