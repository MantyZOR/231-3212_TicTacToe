#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

#include "manager/application.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Application manager;
    manager.setupConnections();

    Renderer::getInstance().render();

    return app.exec();
}
