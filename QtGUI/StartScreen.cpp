#include "StartScreen.hpp"

StartScreen::StartScreen(QWidget* parent) : QWidget(parent)
{
    QPalette pal = palette();
    pal.setBrush(QPalette::Background, QColor(255,255,255));
    setAutoFillBackground(true);
    setPalette(pal);
};
