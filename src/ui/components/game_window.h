#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H


#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

class TicTacToeWindow : public QWidget
{
    Q_OBJECT

public:
    TicTacToeWindow(QWidget *parent = nullptr)
    : QWidget(parent)
    {
        // Create a grid layout
        QGridLayout *gridLayout = new QGridLayout(this);

        // Create the TicTacToe buttons
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                QPushButton *button = new QPushButton(this);
                button->setFixedSize(60, 60);
                gridLayout->addWidget(button, i, j);
                connect(button, &QPushButton::clicked, this, &TicTacToeWindow::handleButtonClicked);
                buttons[i][j] = button;
            }
        }

        // Create the status label
        statusLabel = new QLabel("Player X's turn", this);
        gridLayout->addWidget(statusLabel, 3, 0, 1, 3);

        leaveButton.setText("Leave");
        leaveButton.setParent(this);
        gridLayout->addWidget(&leaveButton, 4, 0, 1, 3);
        connect(&leaveButton, &QPushButton::clicked, this, &TicTacToeWindow::leaveButtonClicked);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                buttons[i][j]->setEnabled(false);
            }
        }
    }

    QPushButton* getLeaveButton()
    {
        return &leaveButton;
    }


    void enableButtons();
    void fill_buttons(const std::vector<std::vector<int>>& state)
    {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                auto button = buttons[i][j];
                auto cell = state[i][j];
                if (cell == 0) {
                    button->setText("");
                } else if (cell == 1) {
                    button->setText("X");
                } else {
                    button->setText("O");
                }
            }
        }
    }
    bool check_winner();

    signals:
        void leaveGame();

    private slots:
        void handleButtonClicked();

    void leaveButtonClicked()
    {
        emit leaveGame();
    }

private:
    void checkForWinner()
    {
        // Check for a winner and update the status label if there is one
        // This is left as an exercise for the reader
    }

    QLabel *statusLabel;
    QPushButton leaveButton;
    QPushButton *buttons[3][3];  // 2D array to store the buttons
};


#endif //GAME_WINDOW_H
