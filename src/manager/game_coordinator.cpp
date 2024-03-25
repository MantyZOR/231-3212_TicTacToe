#include "game_coordinator.h"

std::vector<std::vector<int>> GameCoordinator::stringToState(const std::string& stateStr)
{
    std::vector state(3, std::vector(3, 0));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            state[i][j] = stateStr[i * 3 + j] - '0';
        }
    }
    return state;
}

std::string GameCoordinator::stateToString() const
{
    std::string stateStr;
    for (const auto& row : state) {
        for (int cell : row) {
            stateStr += std::to_string(cell);
        }
    }
    return stateStr;
}

// Getters
int GameCoordinator::getAuthorizeID() const {
    return authorizeID;
}

int GameCoordinator::getGameId() const {
    return game.id;
}

int GameCoordinator::getPlayerX() const {
    return game.playerX;
}

int GameCoordinator::getPlayerO() const {
    return game.playerO;
}

int GameCoordinator::getCurrentPlayer() const {
    return game.currentPlayer;
}

int GameCoordinator::getWinner() const {
    return game.winner;
}

std::string GameCoordinator::getState() const {
    return game.state;
}

// Setters
void GameCoordinator::setAuthorizeID(int authorizeID) {
    this->authorizeID = authorizeID;
}

void GameCoordinator::setGameId(int gameId) {
    this->game.id = gameId;
}

void GameCoordinator::setPlayerX(int playerX) {
    this->game.playerX = playerX;
}

void GameCoordinator::setPlayerO(int playerO) {
    this->game.playerO = playerO;
}

void GameCoordinator::setCurrentPlayer(int currentPlayer) {
    this->game.currentPlayer = currentPlayer;
}

void GameCoordinator::setWinner(int winner) {
    this->game.winner = winner;
}

void GameCoordinator::setState(const std::vector<std::vector<int>>& state) {
    this->state = state;
}

void GameCoordinator::setGame(Game game)
{
    this->game = game;
}
