#include "myglwidget.h"

MyGLWidget::MyGLWidget(QWidget* parent):QOpenGLWidget(parent)
{

}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.5 , 0.7, 1.0, 1.0);
}

void MyGLWidget::paintGL()
{

}

void MyGLWidget::resizeGL(int width, int height)
{

}

void MyGLWidget::changeToRed(int value)
{
    makeCurrent(); // Hace que el contexto gráfico sea el actual
    glClearColor((float)value / 100, 0.0, 0.0, 1.0);
    update();
}
