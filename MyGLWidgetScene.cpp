#include "MyGLWidgetScene.h"
#include <iostream>

MyGLWidgetScene::MyGLWidgetScene(QWidget* parent):QOpenGLWidget(parent)
{
    myLightPos = glm::vec3(-0.5f, 1.5f, 2.0f);
    myLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    myAtt = 0.9f;
}

MyGLWidgetScene::~MyGLWidgetScene()
{

}


void MyGLWidgetScene::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    scene.loadScene();
}

void MyGLWidgetScene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    scene.render(myLightPos, myLightColor, myAtt);
}

void MyGLWidgetScene::resizeGL(int width, int height)
{
    float ratio = (float)width / (float)height;
    scene.getCamera().setAspectRatio(ratio);
}

void MyGLWidgetScene::keyPressEvent(QKeyEvent *e)
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
        case Qt::Key_Left:
            scene.getCamera().orbitY(-angle);
            break;
        case Qt::Key_Right:
            scene.getCamera().orbitY(angle);
            break;
        case Qt::Key_Up:
            scene.getCamera().orbitX(-angle);
            break;
        case Qt::Key_Down:
            scene.getCamera().orbitX(angle);
            break;
        case Qt::Key_Z:
            scene.getCamera().orbitZ(-angle);
            break;
        case Qt::Key_X:
            scene.getCamera().orbitZ(angle);
            break;
        default:
            e->ignore();
            return;
    }

    update();
}

// Light position: slider sends ×10 values, we divide by 10 for float precision
void MyGLWidgetScene::setLightPosX(int x) { myLightPos.x = x / 10.0f; update(); }
void MyGLWidgetScene::setLightPosY(int y) { myLightPos.y = y / 10.0f; update(); }
void MyGLWidgetScene::setLightPosZ(int z) { myLightPos.z = z / 10.0f; update(); }

void MyGLWidgetScene::setLightColorR(int r) { myLightColor.r = r / 255.0f; update(); }
void MyGLWidgetScene::setLightColorG(int g) { myLightColor.g = g / 255.0f; update(); }
void MyGLWidgetScene::setLightColorB(int b) { myLightColor.b = b / 255.0f; update(); }

// Attenuation: slider sends 0-100, we divide by 100 for 0.0-1.0
void MyGLWidgetScene::setAtt(int value) { myAtt = value / 100.0f; update(); }

void MyGLWidgetScene::activeTexture(bool isTextureActive)
{
    makeCurrent();

    std::vector<ModelInstance*> instances = scene.getInstances();

    for (ModelInstance* model : instances)
    {
        model->setTextureActive(isTextureActive);
    }

    update();
}

void MyGLWidgetScene::activeBump(bool isBumpActive)
{
    makeCurrent();

    std::vector<ModelInstance*> instances = scene.getInstances();

    for (ModelInstance* model : instances)
    {
        model->setBumpActive(isBumpActive);
    }

    update();
}
