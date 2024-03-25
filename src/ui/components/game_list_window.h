#ifndef GAME_LIST_WINDOW_H
#define GAME_LIST_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>

struct GameUI {
    int id;
    std::string name;
};

class GameListWindow : public QWidget
{
    Q_OBJECT

public:
    GameListWindow(QWidget* parent = nullptr);
    void FillGames(const std::vector<GameUI>& games);
    int getSelectedGameId() const;

    QVBoxLayout layout;
    QLineEdit   inputSearch;
    QListWidget listGames;
    QPushButton buttonConnect;
    QPushButton buttonCreateRoom;
    std::vector<GameUI> games;
};

#endif //GAME_LIST_WINDOW_H
