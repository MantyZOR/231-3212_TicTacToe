#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "components/auth_window.h"
#include "components/game_list_window.h"
#include "components/game_window.h"

class Renderer
{
public:
    static Renderer& getInstance();
    void             render();

    AuthWindow* getAuthWindow();
    GameListWindow* getGameListWindow();
    TicTacToeWindow* getTicTacToeWindow();
    void changeWindow(QWidget* window);

private:
    Renderer();
    QWidget window;
    QVBoxLayout layout;
    QStackedWidget stackedWidget;

    AuthWindow authWindow;
    GameListWindow gameListWindow;
    TicTacToeWindow ticTacToeWindow;
};

#endif //RENDERER_H
