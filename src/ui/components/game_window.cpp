#include "game_window.h"

#include "src/manager/application.h"
#include "src/manager/game_coordinator.h"

void TicTacToeWindow::handleButtonClicked()
{
    if (GameCoordinator::getInstance().getWinner() != -1)
    {
        statusLabel->setText(QString("Player %1 won!").
                             arg(GameCoordinator::getInstance().getWinner() == GameCoordinator::getInstance().getPlayerX()? "X" : "O"));
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                buttons[i][j]->setEnabled(false);
            }
        }
        return;
    }

    if (GameCoordinator::getInstance().getPlayerX() == -1 || GameCoordinator::getInstance().getPlayerO() == -1)
    {
        statusLabel->setText(QString("Waiting second player"));
        return;
    }

    if (GameCoordinator::getInstance().getCurrentPlayer() != GameCoordinator::getInstance().getAuthorizeID())
        return;


    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button->text().isEmpty())
    {
        auto is_player_x = GameCoordinator::getInstance().getAuthorizeID() == GameCoordinator::getInstance().getPlayerX();
        auto text        = is_player_x ? "X" : "O";
        button->setText(text);
        statusLabel->setText(QString("Player %1's turn").arg(text));


        // Find the index of the clicked button
        auto result = -1; // it should be the index of the button in the 2D array sum
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (buttons[i][j] == button)
                {
                    qDebug() << "Button at index" << i << j << "was clicked";
                    result = i * 3 + j;
                }
            }
        }

        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                buttons[i][j]->setEnabled(false);
            }
        }

        Application::getInstance().sendAction(result);
    }
}

void TicTacToeWindow::enableButtons()
{
    if (GameCoordinator::getInstance().getWinner() != -1)
    {
        statusLabel->setText(QString("Player %1 won!").
                             arg(GameCoordinator::getInstance().getWinner() == GameCoordinator::getInstance().getPlayerX() ? "X" : "O"));
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                buttons[i][j]->setEnabled(false);
            }
        }
        return;
    }

    if (GameCoordinator::getInstance().getPlayerX() == -1 || GameCoordinator::getInstance().getPlayerO() == -1)
    {
        statusLabel->setText(QString("Waiting second player"));
        return;
    }

    auto is_player_x = GameCoordinator::getInstance().getCurrentPlayer() == GameCoordinator::getInstance().getPlayerX();
    auto text        = is_player_x ? "X" : "O";
    statusLabel->setText(QString("Player %1's turn").arg(text));

    if (GameCoordinator::getInstance().getCurrentPlayer() != GameCoordinator::getInstance().getAuthorizeID())
        return;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            buttons[i][j]->setEnabled(true);
        }
    }
}
