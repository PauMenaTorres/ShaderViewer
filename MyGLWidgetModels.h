#ifndef MYGLWIDGETMODELS_H
#define MYGLWIDGETMODELS_H

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

class MyGLWidgetModels : public QOpenGLWidget, QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

    public:
        MyGLWidgetModels(QWidget *parent=0);
        ~MyGLWidgetModels();

protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        virtual void keyPressEvent(QKeyEvent *e);

    private:
        Scene scene;

        void loadShaders();
        float halfVP;
        vec2 resolution;

        QOpenGLShaderProgram* program;

        vec3 myLightPos;
        vec3 myLightColor;

    public slots:
        void setLightPosX(int x);
        void setLightPosY(int y);
        void setLightPosZ(int z);

        void setLightColorR(int r);
        void setLightColorG(int g);
        void setLightColorB(int b);

        void activeTexture(bool isTextureActive);
        void activeBump(bool isBumpActive);
};

#endif // MYGLWIDGETMODELS_H
