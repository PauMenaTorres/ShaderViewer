QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

INCLUDEPATH+=C:/Qt/Qt5.12.12/Tools/glm-master

# Assimp Dependency
INCLUDEPATH += $$PWD/thirdparty/assimp/include
LIBS += $$PWD/thirdparty/assimp/lib/Assimp32.dll

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Camera.cpp \
    ModelResource.cpp \
    ModelInstance.cpp \
    MyGLWidgetSpheres.cpp \
    MyGLWidgetModels.cpp \
    MyGLWidgetScene.cpp \
    MyLabel.cpp \
    MyLineEdit.cpp \
    Scene.cpp \
    main.cpp \
    model.cpp \
    widgetMain.cpp \
    widgetModels.cpp \
    widgetScene.cpp \
    widgetSpheres.cpp \
    WaterFrameBuffers.cpp

HEADERS += \
    Camera.h \
    ModelResource.h \
    ModelInstance.h \
    MyGLWidgetSpheres.h \
    MyGLWidgetModels.h \
    MyGLWidgetScene.h \
    MyLabel.h \
    MyLineEdit.h \
    Scene.h \
    model.h \
    widgetMain.h \
    widgetModels.h \
    widgetScene.h \
    widgetSpheres.h \
    WaterFrameBuffers.h

FORMS += \
    widgetMain.ui \
    widgetModels.ui \
    widgetScene.ui \
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
    geometryPass.frag \
    lightingPass.frag \
    lightingPass.vert \
    vertex.vert \
    vertexModel.vert
