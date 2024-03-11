#include "application.h"
#include <QDateTime>

Application::Application() : client(Client::getInstance()),
                             renderer(Renderer::getInstance())
{
}

void Application::setupConnections()
{
    QObject::connect(renderer.getButton(),
                     &QPushButton::clicked,
                     [&]()
                     {
                         client.connectToHost("127.0.0.1", 8083);
                     });

    QObject::connect(client.getSocket(),
                     &QTcpSocket::connected,
                     [&]()
                     {
                         client.writeData(renderer.getInput()->text());
                     });

    QObject::connect(client.getSocket(),
                     &QTcpSocket::readyRead,
                     [&]()
                     {
                         QByteArray data        = client.readData();
                         QString    currentTime = QDateTime::currentDateTime().toString();
                         renderer.getLabel()->setText("Server says: " + QString(data) + " at " + currentTime);
                     });
}
