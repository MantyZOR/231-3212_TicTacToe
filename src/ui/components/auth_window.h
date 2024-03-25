#ifndef AUTH_WINDOW_H
#define AUTH_WINDOW_H
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class AuthWindow : public QWidget
{
    Q_OBJECT

public:
    AuthWindow(QWidget* parent = nullptr);

    QVBoxLayout layout;
    QLabel      labelUsername;
    QLineEdit   inputUsername;
    QLabel      labelPassword;
    QLineEdit   inputPassword;
    QPushButton buttonLogin;
    QPushButton buttonRegister;
};


#endif //AUTH_WINDOW_H
