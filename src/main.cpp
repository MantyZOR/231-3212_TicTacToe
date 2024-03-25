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

    Application::getInstance().setupConnections();

    QObject::connect(&app, &QApplication::aboutToQuit, &Application::getInstance(), &Application::onAboutToQuit);

    Renderer::getInstance().render();

    return app.exec();
}
