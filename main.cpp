#include <QApplication>
#include "widgetMain.h"
#include "widgetSpheres.h"
#include "widgetModels.h"
#include "widgetScene.h"
#include <QSurfaceFormat>

int main(int argc, char *argv[])

{

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QApplication a(argc, argv);

    QSurfaceFormat f;

    f.setVersion(4,3);

    f.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(f);

    WidgetScene w;

    w.show();

    return a.exec();

}
