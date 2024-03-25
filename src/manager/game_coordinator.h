#ifndef GAME_COORDINATOR_H
#define GAME_COORDINATOR_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

enum class CellState
{
    EMPTY = 0,
    X = 1,
    O = 2
};


struct Game
{
    int id = -1;
    int playerX = -1;
    int playerO = -1;
    int currentPlayer = -1;
    int winner = -1;
    std::string state = "000000000";
    std::string roomName;
};

using json = nlohmann::json;

inline void to_json(json& j, const Game& g)
{
    j = json{{"id", g.id}, {"playerX", g.playerX}, {"playerO", g.playerO}, {"currentPlayer", g.currentPlayer}, {"winner", g.winner}, {"state", g.state}, {"roomName", g.roomName}};
}

inline void from_json(const json& j, Game& g)
{
    j.at("id").get_to(g.id);
    j.at("playerX").get_to(g.playerX);
    j.at("playerO").get_to(g.playerO);
    j.at("currentPlayer").get_to(g.currentPlayer);
    j.at("winner").get_to(g.winner);
    j.at("state").get_to(g.state);
    j.at("roomName").get_to(g.roomName);
}

class GameCoordinator {
    int authorizeID = -1;
    std::vector<std::vector<int>> state {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    };
    Game game;

    GameCoordinator() = default;

public:
    GameCoordinator(const GameCoordinator&) = delete;
    GameCoordinator& operator=(const GameCoordinator&) = delete;

    static GameCoordinator& getInstance() {
        static GameCoordinator instance{};
        return instance;
    }

    std::vector<std::vector<int>> stringToState(const std::string& stateStr);
    std::string stateToString() const;

    // Getters
    int getAuthorizeID() const;
    int getGameId() const;
    int getPlayerX() const;
    int getPlayerO() const;
    int getCurrentPlayer() const;
    int getWinner() const;
    std::string getState() const;

    // Setters
    void setAuthorizeID(int authorizeID);
    void setGameId(int gameId);
    void setPlayerX(int playerX);
    void setPlayerO(int playerO);
    void setCurrentPlayer(int currentPlayer);
    void setWinner(int winner);
    void setState(const std::vector<std::vector<int>>& state);
    void setGame(Game game);
};

#endif //GAME_COORDINATOR_H
