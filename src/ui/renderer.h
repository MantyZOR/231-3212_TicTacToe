#ifndef RENDERER_H
#define RENDERER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class Renderer
{
public:
    static Renderer& getInstance();
    void             render();

    QLabel*      getLabel();
    QPushButton* getButton();
    QLineEdit*   getInput();

private:
    Renderer();
    QWidget     window;
    QVBoxLayout layout;
    QLabel      label {"Welcome to Tic Tac Toe!"};
    QPushButton button {"Send Message"};
    QLineEdit   input;
};

#endif //RENDERER_H
