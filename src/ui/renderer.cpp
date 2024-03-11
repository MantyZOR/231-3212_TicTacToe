#include "renderer.h"

Renderer& Renderer::getInstance()
{
    static Renderer instance;
    return instance;
}

Renderer::Renderer() : layout(&window)
{
    window.setWindowTitle("Tic Tac Toe");
    input.setToolTip("Enter message here...");
    layout.addWidget(&label);
    layout.addWidget(&input);
    layout.addWidget(&button);
    window.setLayout(&layout);
}

void Renderer::render()
{
    window.show();
}

QLabel* Renderer::getLabel()
{
    return &label;
}

QPushButton* Renderer::getButton()
{
    return &button;
}

QLineEdit* Renderer::getInput()
{
    return &input;
}
