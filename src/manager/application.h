#ifndef APPLICATION_H
#define APPLICATION_H
#include "src/network/client.h"
#include "src/ui/renderer.h"

class Application
{
public:
    Application();
    void setupConnections();

private:
    Client&   client;
    Renderer& renderer;
};

#endif //APPLICATION_H
