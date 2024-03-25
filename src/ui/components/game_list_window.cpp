#include "game_list_window.h"

#include <QMessageBox>

GameListWindow::GameListWindow(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    // layout->addWidget(&inputSearch);
    layout->addWidget(&listGames);
    layout->addWidget(&buttonConnect);
    layout->addWidget(&buttonCreateRoom);
    // inputSearch.setPlaceholderText("Search games...");
    buttonConnect.setText("Connect");
    buttonCreateRoom.setText("Create Game");

    setLayout(layout);
}

void GameListWindow::FillGames(const std::vector<GameUI>& games)
{
    this->games = games;
    listGames.clear();
    for (const auto& [id, name] : games)
    {
        listGames.addItem(QString::fromStdString(name));
    }
}

int GameListWindow::getSelectedGameId() const
{
    if (listGames.currentRow() < 0)
        return -1;

    return games[listGames.currentRow()].id;
}
