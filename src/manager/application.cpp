#include "application.h"
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>

#include <nlohmann/json.hpp>

#include "game_coordinator.h"
#include "iostream"

Application::Application() : client(Client::getInstance()),
                             renderer(Renderer::getInstance())
{
}

enum class STATUS_CODES
{
    REGISTER     = 0,
    AUTHENTICATE = 1,
    CREATE_ROOM  = 2,
    JOIN_ROOM    = 3,
    LEAVE_ROOM   = 4,
    GET_ROOMS    = 5,
    ACTION       = 6,
    GET_GAME     = 7,
};

Application& Application::getInstance()
{
    static Application instance{};
    return instance;
}

void Application::setupConnections()
{
    static bool in_box = false;
    QObject::connect(&renderer.getAuthWindow()->buttonRegister,
                     &QPushButton::clicked,
                     [&]()
                     {
                         if (client.getSocket()->state() != QAbstractSocket::ConnectedState)
                         {
                             client.connectToHost("127.0.0.1", 8083);
                         }
                         nlohmann::json json;
                         json["type"]     = STATUS_CODES::REGISTER;
                         json["login"]    = renderer.getAuthWindow()->inputUsername.text().toStdString();
                         json["password"] = renderer.getAuthWindow()->inputPassword.text().toStdString();
                         client.writeData(json.dump().c_str());
                     });

    QObject::connect(&renderer.getAuthWindow()->buttonLogin,
                     &QPushButton::clicked,
                     [&]()
                     {
                         if (client.getSocket()->state() != QAbstractSocket::ConnectedState)
                         {
                             client.connectToHost("127.0.0.1", 8083);
                         }
                         nlohmann::json json;
                         json["type"]     = STATUS_CODES::AUTHENTICATE;
                         json["login"]    = renderer.getAuthWindow()->inputUsername.text().toStdString();
                         json["password"] = renderer.getAuthWindow()->inputPassword.text().toStdString();
                         client.writeData(json.dump().c_str());
                     });


    QObject::connect(renderer.getTicTacToeWindow()->getLeaveButton(),
                     &QPushButton::released,
                     [&]()
                     {
                         client.connectToHost("127.0.0.1", 8083);

                         if (client.getSocket()->waitForConnected())
                         {
                             std::cout << "Connected to server" << std::endl;
                             nlohmann::json json;
                             json["type"]     = STATUS_CODES::LEAVE_ROOM;
                             json["playerId"] = GameCoordinator::getInstance().getAuthorizeID();
                             json["gameId"]   = GameCoordinator::getInstance().getGameId();

                             client.writeData(json.dump().c_str());

                             // Wait for the data to be written
                             if (client.getSocket()->waitForBytesWritten(3000))
                             {
                                 std::cout << "Data sent to server" << std::endl;
                             }
                             else
                             {
                                 std::cout << "Failed to send data to server" << std::endl;
                             }
                         }
                     });


    QObject::connect(&renderer.getGameListWindow()->buttonCreateRoom,
                     &QPushButton::clicked,
                     [&]()
                     {
                         in_box = true;
                         // Check if the socket is already connected before trying to connect
                         if (client.getSocket()->state() != QAbstractSocket::ConnectedState)
                         {
                             client.connectToHost("127.0.0.1", 8083);
                         }

                         nlohmann::json json;
                         QMessageBox    msgBox;
                         msgBox.setWindowTitle("Choose your symbol");
                         msgBox.setText("Do you want to be X or O?");
                         QAbstractButton* pButtonX = msgBox.addButton("X", QMessageBox::YesRole);
                         msgBox.addButton("O", QMessageBox::NoRole);


                         QObject::connect(&msgBox,
                                          &QMessageBox::finished,
                                          [&]()
                                          {
                                              json["type"]       = STATUS_CODES::CREATE_ROOM;
                                              json["playerID"]   = GameCoordinator::getInstance().getAuthorizeID();
                                              json["playerType"] = msgBox.clickedButton() == pButtonX
                                                                       ? CellState::X
                                                                       : CellState::O;

                                              client.writeData(json.dump().c_str());
                                              in_box = false;
                                          });

                         msgBox.exec();
                     });


    QObject::connect(&renderer.getGameListWindow()->buttonConnect,
                     &QPushButton::clicked,
                     [&]()
                     {
                         if (client.getSocket()->state() != QAbstractSocket::ConnectedState)
                         {
                             client.connectToHost("127.0.0.1", 8083);
                         }

                         if (renderer.getGameListWindow()->getSelectedGameId() == -1)
                         {
                             QMessageBox::warning(nullptr, "Error", "Please select a game");
                             return;
                         }

                         nlohmann::json json;
                         json["type"]     = STATUS_CODES::JOIN_ROOM;
                         json["playerID"] = GameCoordinator::getInstance().getAuthorizeID();
                         json["gameID"]   = renderer.getGameListWindow()->getSelectedGameId();
                         client.writeData(json.dump().c_str());
                     });

    static QTimer timer;
    static QTimer timer_get_game;

    if (static auto once = true; once)
    {
        timer.setInterval(3000);
        timer.start();

        timer_get_game.setInterval(503);
        timer_get_game.start();
        once = !once;
    }

    QObject::connect(&timer,
                     &QTimer::timeout,
                     [&]()
                     {
                         if (in_box)
                             return;

                         if (client.getSocket()->state() != QAbstractSocket::ConnectedState)
                         {
                             client.connectToHost("127.0.0.1", 8083);
                         }
                         nlohmann::json json;
                         json["type"] = STATUS_CODES::GET_ROOMS;
                         client.writeData(json.dump().c_str());
                     });

    QObject::connect(&timer_get_game,
                     &QTimer::timeout,
                     [&]()
                     {
                         if (in_box || GameCoordinator::getInstance().getGameId() == -1)
                             return;

                         if (client.getSocket()->state() != QAbstractSocket::ConnectedState)
                         {
                             client.connectToHost("127.0.0.1", 8083);
                         }
                         nlohmann::json json;
                         json["type"]   = STATUS_CODES::GET_GAME;
                         json["gameID"] = GameCoordinator::getInstance().getGameId();
                         client.writeData(json.dump().c_str());
                     });


    QObject::connect(client.getSocket(),
                     &QTcpSocket::readyRead,
                     [&]()
                     {
                         QByteArray data        = client.readData();
                         QString    currentTime = QDateTime::currentDateTime().toString();
                         std::cout << ("Server says: " + QString(data) + " at " + currentTime).toStdString() <<
                                 std::endl;

                         nlohmann::json message = nlohmann::json::parse(data.toStdString());

                         auto type = static_cast<STATUS_CODES>(message["type"].get<int>());

                         switch (type)
                         {
                             case STATUS_CODES::AUTHENTICATE:
                                 {
                                     auto result = message["result"].get<int>();
                                     if (result != -1)
                                     {
                                         GameCoordinator::getInstance().setAuthorizeID(result);
                                         renderer.changeWindow(renderer.getGameListWindow());
                                     }
                                     else
                                     {
                                         QMessageBox::warning(nullptr, "Error", "Invalid username or password");
                                     }
                                     break;
                                 }
                             case STATUS_CODES::REGISTER:
                                 {
                                     auto result = message["result"].get<int>();
                                     if (result != -1)
                                     {
                                         GameCoordinator::getInstance().setAuthorizeID(result);
                                         renderer.changeWindow(renderer.getGameListWindow());
                                     }
                                     else
                                     {
                                         QMessageBox::warning(nullptr, "Error", "Cant register user");
                                     }

                                     break;
                                 }
                             case STATUS_CODES::GET_ROOMS:
                                 {
                                     auto result = message["result"];

                                     std::vector<GameUI> rooms;
                                     for (const auto& room : result)
                                     {
                                         Game game = room.get<Game>();

                                         GameUI gameUI;
                                         gameUI.id   = game.id;
                                         gameUI.name = game.roomName;
                                         rooms.emplace_back(gameUI);
                                     }

                                     renderer.getGameListWindow()->FillGames(rooms);
                                     break;
                                 }
                             case STATUS_CODES::CREATE_ROOM:
                                 {
                                     auto game = message["result"].get<Game>();
                                     GameCoordinator::getInstance().setGame(game);
                                     renderer.changeWindow(renderer.getTicTacToeWindow());
                                     break;
                                 }
                             case STATUS_CODES::JOIN_ROOM:
                                 {
                                     auto game = message["result"].get<Game>();
                                     GameCoordinator::getInstance().setGame(game);
                                     renderer.changeWindow(renderer.getTicTacToeWindow());
                                     break;
                                 }
                             case STATUS_CODES::GET_GAME:
                                 {
                                     auto game = message["result"].get<Game>();
                                     GameCoordinator::getInstance().setGame(game);

                                     renderer.getTicTacToeWindow()->enableButtons();

                                     auto stringToState = [&](
                                         const std::string& stateStr) -> std::vector<std::vector<int>>
                                     {
                                         std::vector state(3, std::vector(3, 0));
                                         for (int i = 0; i < 3; ++i)
                                         {
                                             for (int j = 0; j < 3; ++j)
                                             {
                                                 state[i][j] = stateStr[i * 3 + j] - '0';
                                             }
                                         }
                                         return state;
                                     };

                                     renderer.getTicTacToeWindow()->
                                              fill_buttons(stringToState(GameCoordinator::getInstance().getState()));
                                     // todo: renderer.getTicTacToeWindow()->updateGame();
                                     break;
                                 }
                             case STATUS_CODES::ACTION:
                                 {
                                     auto game = message["result"].get<Game>();
                                     GameCoordinator::getInstance().setGame(game);
                                     renderer.getTicTacToeWindow()->enableButtons();

                                     auto stringToState = [&](
                                         const std::string& stateStr) -> std::vector<std::vector<int>>
                                     {
                                         std::vector state(3, std::vector(3, 0));
                                         for (int i = 0; i < 3; ++i)
                                         {
                                             for (int j = 0; j < 3; ++j)
                                             {
                                                 state[i][j] = stateStr[i * 3 + j] - '0';
                                             }
                                         }
                                         return state;
                                     };

                                     renderer.getTicTacToeWindow()->
                                              fill_buttons(stringToState(GameCoordinator::getInstance().getState()));
                                     break;
                                 }
                             case STATUS_CODES::LEAVE_ROOM:
                                 {
                                     renderer.changeWindow(renderer.getGameListWindow());
                                     break;
                                 }
                             default:
                                 std::cout << "Unknown message type" << std::endl;
                                 break;
                         }
                     });
}

void Application::onAboutToQuit()
{
    client.connectToHost("127.0.0.1", 8083);

    if (client.getSocket()->waitForConnected())
    {
        std::cout << "Connected to server" << std::endl;
        nlohmann::json json;
        json["type"]     = STATUS_CODES::LEAVE_ROOM;
        json["playerId"] = GameCoordinator::getInstance().getAuthorizeID();
        json["gameId"]   = GameCoordinator::getInstance().getGameId();

        client.writeData(json.dump().c_str());

        // Wait for the data to be written
        if (client.getSocket()->waitForBytesWritten(3000))
        {
            std::cout << "Data sent to server" << std::endl;
        }
        else
        {
            std::cout << "Failed to send data to server" << std::endl;
        }
    }
}

void Application::sendAction(const int cell)
{
    client.connectToHost("127.0.0.1", 8083);

    if (client.getSocket()->waitForConnected())
    {
        std::cout << "Connected to server" << std::endl;
        nlohmann::json json;
        json["type"]     = STATUS_CODES::ACTION;
        json["playerId"] = GameCoordinator::getInstance().getAuthorizeID();
        json["gameId"]   = GameCoordinator::getInstance().getGameId();
        json["cell"]     = cell;

        client.writeData(json.dump().c_str());

        // Wait for the data to be written
        if (client.getSocket()->waitForBytesWritten(3000))
        {
            std::cout << "Action sent to server" << std::endl;
        }
        else
        {
            std::cout << "Failed to send data to server" << std::endl;
        }
    }
}
