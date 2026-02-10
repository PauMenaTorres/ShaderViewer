#include "widget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QPushButton>
#include <QFrame>
#include <glm/glm.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFrame f(0, NULL), g(0, NULL);
    QPushButton hello("Hello QT", &f);
    QPushButton hello2("BYE QT", &g);
    glm::vec3 v(1.0f, 1.0f, 1.0f),w;
    w=3.0f * v;

    a.connect(&hello, SIGNAL(clicked()), &g, SLOT(close()));

    hello.resize(100, 30);
    hello2.resize(200, 60);
    f.show();
    g.show();


    return a.exec();
}
