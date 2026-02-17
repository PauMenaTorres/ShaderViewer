#include <QApplication>
#include "widget.h"
#include <QSurfaceFormat>

int main(int argc, char *argv[])

{

    QApplication a(argc, argv);

    QSurfaceFormat f;

    f.setVersion(4,3);

    f.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(f);

    Widget w;

    w.show();

    return a.exec();

}
