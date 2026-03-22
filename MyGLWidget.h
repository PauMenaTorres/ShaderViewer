#ifndef MYGLWIDGET_H

#define MYGLWIDGET_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>

#include <glm/glm.hpp>

using namespace glm;

struct SphereData {
    vec3 center;
    float radius;
    vec4 color;
};

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_3_Core

{
    Q_OBJECT

    public:

        MyGLWidget(QWidget *parent=0);
        ~MyGLWidget();

    protected:

        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        virtual void mousePressEvent ( QMouseEvent *e);
        //virtual void mouseReleaseEvent ( QMouseEvent *e);
        //virtual void mouseMoveEvent ( QMouseEvent *e);
        virtual void keyPressEvent ( QKeyEvent *e);

    private:

        void createBuffers();
        void createBuffers2();
        void loadShaders();

        float halfVP;
        vec2 mousePos;
        vec2 resolution;

        GLuint vertexLoc, colorLoc, scaleLoc, idShaderLoc, halfLoc, resolutionLoc, mousePosLoc, VAO1;
        QOpenGLShaderProgram* program;

        std::vector<SphereData> mySpheres;

    public slots:

        void changeToRed(int value);
        void showState(int s);
        void changeToButton1();
        void changeToButton2();
        void changeToButton3();
        void changeToButton4();

        void addSphere(vec3 C, float r, vec4 color);
        void sendSpheresToShader();


};

#endif // MYGLWIDGET_H
