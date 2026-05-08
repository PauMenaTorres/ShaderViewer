#ifndef MyGLWidgetScene_H
#define MyGLWidgetScene_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QKeyEvent>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <model.h>
#include <Scene.h>

using namespace glm;

class MyGLWidgetScene : public QOpenGLWidget, QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

    public:
        MyGLWidgetScene(QWidget *parent=0);
        ~MyGLWidgetScene();

protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        virtual void keyPressEvent(QKeyEvent *e);

    private:
        Scene scene;

        float halfVP;
        vec2 resolution;

        vec3 myLightPos;
        vec3 myLightColor;
        float myAtt = 0.9f;

    public slots:
        void setLightPosX(int x);
        void setLightPosY(int y);
        void setLightPosZ(int z);

        void setLightColorR(int r);
        void setLightColorG(int g);
        void setLightColorB(int b);

        void setAtt(int value);

        void activeTexture(bool isTextureActive);
        void activeBump(bool isBumpActive);
};

#endif // MyGLWidgetScene_H
