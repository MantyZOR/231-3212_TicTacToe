#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Tic Tac Toe");

    auto layout = new QVBoxLayout(&window);
    auto label = new QLabel("Waiting for server response...");
    auto button = new QPushButton("Connect to Server");

    layout->addWidget(label);
    layout->addWidget(button);

    auto socket = std::make_unique<QTcpSocket>(&window);

    QObject::connect(button, &QPushButton::clicked, [socket = socket.get()]() {
        socket->connectToHost("127.0.0.1", 8082);
    });

    QObject::connect(socket.get(), &QTcpSocket::connected, [socket = socket.get()]() {
        socket->write("Hello from client");
    });

    QObject::connect(socket.get(), &QTcpSocket::readyRead, [socket = socket.get(), label]() {
        QByteArray data = socket->readAll();
        QString currentTime = QDateTime::currentDateTime().toString();
        label->setText("Server says: " + QString(data) + " at " + currentTime);
    });

    window.setLayout(layout);
    window.show();

    return app.exec();
}
