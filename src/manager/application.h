#ifndef APPLICATION_H
#define APPLICATION_H
#include "src/network/client.h"
#include "src/ui/renderer.h"

class Application : public QObject
{
    Q_OBJECT
public:
    static Application& getInstance();
    void setupConnections();

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
