#include "renderer.h"

Renderer& Renderer::getInstance()
{
    static Renderer instance;
    return instance;
}

Renderer::Renderer() : layout(&window)
{
    window.setWindowTitle("Tic Tac Toe");

    layout.addWidget(&stackedWidget);
    window.setLayout(&layout);

    stackedWidget.addWidget(&authWindow);
    stackedWidget.addWidget(&gameListWindow);
    stackedWidget.addWidget(&ticTacToeWindow);

    changeWindow(getAuthWindow());
}

void Renderer::render()
{
    window.show();
}

AuthWindow* Renderer::getAuthWindow()
{
    return &authWindow;
}

GameListWindow* Renderer::getGameListWindow()
{
    return &gameListWindow;
}

TicTacToeWindow* Renderer::getTicTacToeWindow()
{
    return &ticTacToeWindow;
}

void Renderer::changeWindow(QWidget* window)
{
    stackedWidget.setCurrentWidget(window);
}
