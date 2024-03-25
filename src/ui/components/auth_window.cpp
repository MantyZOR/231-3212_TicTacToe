#include "auth_window.h"
#include <QFormLayout>

AuthWindow::AuthWindow(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QFormLayout(this);
    layout->addRow("Username:", &inputUsername);
    layout->addRow("Password:", &inputPassword);
    inputPassword.setEchoMode(QLineEdit::Password);
    layout->addWidget(&buttonLogin);
    layout->addWidget(&buttonRegister);
    buttonLogin.setText("Login");
    buttonRegister.setText("Register");

    setLayout(layout);
}
