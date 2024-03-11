#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>

class Client
{
public:
    static Client& getInstance();
    void           connectToHost(const QString& host, quint16 port) const;
    void           writeData(const QString& data);
    QByteArray     readData();
    QTcpSocket*    getSocket() const;

private:
    Client();
    std::unique_ptr<QTcpSocket> socket;
};

#endif //CLIENT_H
