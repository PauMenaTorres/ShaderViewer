#include "MyGLWidgetModels.h"
#include <iostream>

MyGLWidgetModels::MyGLWidgetModels(QWidget* parent):QOpenGLWidget(parent)
{

}

MyGLWidgetModels::~MyGLWidgetModels()
{

}

void MyGLWidgetModels::loadShaders()
{
    program = new QOpenGLShaderProgram(this);
    if (!QFile::exists(":/fragmentModel.frag")) {
        qCritical() << "ERROR: El fitxer :/fragmentModel.frag no existeix als recursos!";
        return;
    }
    if (!QFile::exists(":/vertexModel.vert")) {
        qCritical() << "ERROR: El fitxer :/vertexModel.vert no existeix als recursos!";
        return;
    }
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertexModel.vert")) {
        qCritical() << "Error en Vertex Shader:" << program->log();
    }
    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragmentModel.frag")) {
        qCritical() << "Error en Fragment Shader:" << program->log();
    }
    if (!program->link()) {
        qCritical() << "Error enllaçant el programa:" << program->log();
    }
}

void MyGLWidgetModels::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.76, 0.69, 0.52, 1.0);
    loadShaders();
    scene.init();
}

void MyGLWidgetModels::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();

    glm::mat4 viewMat = scene.getCamera().getViewMatrix();
    glm::mat4 projMat = scene.getCamera().getProjectMatrix();

    GLuint viewLoc = glGetUniformLocation(program->programId(), "view");
    GLuint projLoc = glGetUniformLocation(program->programId(), "proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat[0][0]);

    GLuint lightPosLoc = glGetUniformLocation(program->programId(), "lightPos");
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(myLightPos));

    scene.render();

    program->release();
}

void MyGLWidgetModels::resizeGL(int width, int height)
{
    float ratio = (float)width / (float)height;
    scene.getCamera().setAspectRatio(ratio);
}

void MyGLWidgetModels::keyPressEvent(QKeyEvent *e)
{
    makeCurrent();
    float speed = 0.5f;
    float angle = glm::radians(5.0f);

    switch ( e->key() )
    {
        case Qt::Key_W:
            scene.getCamera().moveForward(speed);
            break;
        case Qt::Key_S:
            scene.getCamera().moveForward(-speed);
            break;
        case Qt::Key_A:
            scene.getCamera().rotate(angle);
            break;
        case Qt::Key_D:
            scene.getCamera().rotate(-angle);
            break;
        case Qt::Key_Q:
            scene.getCamera().moveUp(-speed);
            break;
        case Qt::Key_E:
            scene.getCamera().moveUp(speed);
            break;
        case Qt::Key_R:
            scene.getCamera().restart();
            break;
        default:
            e->ignore();
            return;
    }

    update();
}

void MyGLWidgetModels::setLightPosX(double x) { myLightPos.x = x; update(); }
void MyGLWidgetModels::setLightPosY(double y) { myLightPos.y = y; update(); }
void MyGLWidgetModels::setLightPosZ(double z) { myLightPos.z = z; update(); }
