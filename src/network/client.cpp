#include "client.h"

Client& Client::getInstance()
{
    static Client instance;
    return instance;
}

Client::Client() : socket(std::make_unique<QTcpSocket>())
{
}

void Client::connectToHost(const QString& host, quint16 port) const
{
    socket->connectToHost(host, port);
}

void Client::writeData(const QString& data)
{
    socket->write(data.toUtf8());
}

QByteArray Client::readData()
{
    return socket->readAll();
}

QTcpSocket* Client::getSocket() const
{
    return socket.get();
}
