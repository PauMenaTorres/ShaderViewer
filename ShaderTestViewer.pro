QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

INCLUDEPATH+=C:/Qt/Qt5.12.12/Tools/glm-master

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Camera.cpp \
    ModelOBJ.cpp \
    MyGLWidgetSpheres.cpp \
    MyGLWidgetModels.cpp \
    MyLabel.cpp \
    MyLineEdit.cpp \
    Scene.cpp \
    main.cpp \
    model.cpp \
    widgetMain.cpp \
    widgetModels.cpp \
    widgetSpheres.cpp

HEADERS += \
    Camera.h \
    ModelOBJ.h \
    MyGLWidgetSpheres.h \
    MyGLWidgetModels.h \
    MyLabel.h \
    MyLineEdit.h \
    Scene.h \
    model.h \
    widgetMain.h \
    widgetModels.h \
    widgetSpheres.h

FORMS += \
    widgetMain.ui \
    widgetModels.ui \
    widgetSpheres.ui

TRANSLATIONS += \
    ShaderTestViewer_en_GB.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    fragment.frag \
    fragmentGreen.frag \
    fragmentModel.frag \
    fragmentRed.frag \
    fragmentSphere.frag \
    vertex.vert \
    vertexModel.vert
