#include <QApplication>
#include "widgetMain.h"
#include "widgetSpheres.h"
#include "widgetModels.h"
#include <QSurfaceFormat>

int main(int argc, char *argv[])

{

    QApplication a(argc, argv);

    QSurfaceFormat f;

    f.setVersion(4,3);

    f.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(f);

    WidgetModels w;

    w.show();

    return a.exec();

}
