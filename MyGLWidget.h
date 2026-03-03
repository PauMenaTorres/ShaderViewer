#ifndef MYGLWIDGET_H

#define MYGLWIDGET_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
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

    private:

        void createBuffers();
        void loadShaders();
        GLuint vertexLoc, colorLoc, VAO1;
        QOpenGLShaderProgram* program;

    public slots:

        void changeToRed(int value);



};

#endif // MYGLWIDGET_H
