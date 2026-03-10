#ifndef MYGLWIDGET_H

#define MYGLWIDGET_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>

#include <glm/glm.hpp>

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
        glm::vec2 mousePos;
        glm::vec2 resolution;

        glm::vec3 r0 = glm::vec3(0.0, 0.0, 0.0);
        glm::vec3 rd;
        glm::vec2 uv;

        GLuint vertexLoc, colorLoc, scaleLoc, idShaderLoc, halfLoc, resolutionLoc, mousePosLoc, VAO1;
        QOpenGLShaderProgram* program;

    public slots:

        void changeToRed(int value);
        void showState(int s);
        void changeToButton1();
        void changeToButton2();
        void changeToButton3();
        void changeToButton4();


};

#endif // MYGLWIDGET_H
