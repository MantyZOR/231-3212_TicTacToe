#ifndef APPLICATION_H
#define APPLICATION_H
#include "src/network/client.h"
#include "src/ui/renderer.h"
#include "nlohmann/json.hpp"

class Application : public QObject
{
    Q_OBJECT
public:
    static Application& getInstance();
    void setupConnections();


  /**
   * @brief Handles the AUTHENTICATE status code.
   * @param message The JSON message received from the server.
   */
  void handleAuthenticate(nlohmann::json& message);

  /**
   * @brief Handles the REGISTER status code.
   * @param message The JSON message received from the server.
   */
  void handleRegister(nlohmann::json& message);

  /**
   * @brief Handles the GET_ROOMS status code.
   * @param message The JSON message received from the server.
   */
  void handleGetRooms(nlohmann::json& message);

  /**
   * @brief Handles the CREATE_ROOM status code.
   * @param message The JSON message received from the server.
   */
  void handleCreateRoom(nlohmann::json& message);

  /**
   * @brief Handles the JOIN_ROOM status code.
   * @param message The JSON message received from the server.
   */
  void handleJoinRoom(nlohmann::json& message);

  /**
   * @brief Handles the GET_GAME status code.
   * @param message The JSON message received from the server.
   */
  void handleGetGame(nlohmann::json& message);

  /**
   * @brief Handles the ACTION status code.
   * @param message The JSON message received from the server.
   */
  void handleAction(nlohmann::json& message);

  /**
   * @brief Handles the LEAVE_ROOM status code.
   * @param message The JSON message received from the server.
   */
  void handleLeaveRoom(nlohmann::json& message);

    void onAboutToQuit();

    void sendAction(const int cell);
private:
    Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Client&   client;
    Renderer& renderer;
};

#endif //APPLICATION_H
