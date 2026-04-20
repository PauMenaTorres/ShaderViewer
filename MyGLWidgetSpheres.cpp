#include "MyGLWidgetSpheres.h"
#include <iostream>

MyGLWidgetSpheres::MyGLWidgetSpheres(QWidget* parent):QOpenGLWidget(parent)
{
    myLightPos = vec3(1.0, 1.0, 1.0);
    myLightColor = vec3(1.0, 1.0, 1.0);
    myGlobalAmbient = vec3(0.5, 0.5, 0.5);
    resolution = vec2(800, 600);

    currentCenterTemp = vec3(0.0f, 0.0f, 0.0f);
    currentRadiusTemp = 0.3f;
    currentColorTemp = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    currentKaTemp = vec3(0.2f, 0.2f, 0.2f);
    currentKdTemp = vec3(0.8f, 0.8f, 0.8f);
    currentKsTemp = vec3(1.0f, 1.0f, 1.0f);
    currentShininessTemp = 128.0f;

    SphereData initialSphere;
    initialSphere.center = vec3(0.0f, 0.0f, 0.5f);
    initialSphere.radius = 0.4f;
    initialSphere.color = vec4(0.7f, 0.3f, 0.9f, 1.0f);
    initialSphere.ka = vec3(0.2f, 0.2f, 0.2f);
    initialSphere.kd = vec3(0.7f, 0.3f, 0.9f);
    initialSphere.ks = vec3(1.0f, 1.0f, 1.0f);
    mySpheres.push_back(initialSphere);
}

MyGLWidgetSpheres::~MyGLWidgetSpheres()
{
}

void MyGLWidgetSpheres::createBuffers()
{
    glm::vec3 VertCol[4];
    VertCol[0] = glm::vec3(-1.0, 1.0, -1.0);
    VertCol[1] = glm::vec3(-1.0, -1.0, -1.0);
    VertCol[2] = glm::vec3(1.0, 1.0, -1.0);
    VertCol[3] = glm::vec3(1.0, -1.0, -1.0);

   glGenVertexArrays(1, &VAO1);
   glBindVertexArray(VAO1);

   GLuint VBO;
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(VertCol), VertCol, GL_STATIC_DRAW);

   glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
   glEnableVertexAttribArray(vertexLoc);

   glBindVertexArray(0);
}

void MyGLWidgetSpheres::loadShaders()
{
    program = new QOpenGLShaderProgram(this);
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.vert")) {
        qCritical() << "Error en Vertex Shader:" << program->log();
    }
    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragmentSphere.frag")) {
        qCritical() << "Error en Fragment Shader:" << program->log();
    }
    if (!program->link()) {
        qCritical() << "Error linking program:" << program->log();
    }
    program->bind();
    getShaderLocations();
}

void MyGLWidgetSpheres::getShaderLocations()
{
    vertexLoc = glGetAttribLocation(program->programId(), "vertex");

    scaleLoc = glGetUniformLocation(program->programId(), "screenSize");
    numSpheresLoc = glGetUniformLocation(program->programId(), "numSpheres");
    lightPositionLoc = glGetUniformLocation(program->programId(), "lightPosition");
    lightColorLoc = glGetUniformLocation(program->programId(), "lightColor");
    globalAmbientLoc = glGetUniformLocation(program->programId(), "globalAmbient");
    cameraPosLoc = glGetUniformLocation(program->programId(), "cameraPos");
    globalShininessLoc = glGetUniformLocation(program->programId(), "globalShininess");
    camForwardLoc = glGetUniformLocation(program->programId(), "camForward");
    camRightLoc = glGetUniformLocation(program->programId(), "camRight");
    camUpLoc = glGetUniformLocation(program->programId(), "camUp");
}

void MyGLWidgetSpheres::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.76, 0.69, 0.52, 1.0);
    loadShaders();
    createBuffers();
    scene.getCamera().init(glm::vec3(-0.5f), glm::vec3(0.5f));
    scene.getCamera().moveForward(1.0f);
}

void MyGLWidgetSpheres::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();

    sendLightToShader();
    sendSpheresToShader();

    glm::mat4 V = scene.getCamera().getViewMatrix();
    glm::vec3 right(V[0][0], V[1][0], V[2][0]); 
    glm::vec3 up(V[0][1], V[1][1], V[2][1]);
    glm::vec3 forward(-V[0][2], -V[1][2], -V[2][2]);

    glm::mat4 invV = glm::inverse(V);
    glm::vec3 obs = glm::vec3(invV[3]);

    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(obs));
    glUniform3fv(camForwardLoc, 1, glm::value_ptr(forward));
    glUniform3fv(camRightLoc, 1, glm::value_ptr(right));
    glUniform3fv(camUpLoc, 1, glm::value_ptr(up));

    glUniform1f(globalShininessLoc, currentShininessTemp);

    glBindVertexArray(VAO1);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    program->release();
}

void MyGLWidgetSpheres::resizeGL(int width, int height)
{
    glm::vec2 screenSize(width, height);
    program->bind();
    glUniform2fv(scaleLoc, 1, &screenSize[0]);
    program->release();
    resolution = glm::vec2(width, height);

    float ratio = (float)width / (float)height;
    scene.getCamera().setAspectRatio(ratio);
}

void MyGLWidgetSpheres::keyPressEvent(QKeyEvent *e)
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

void MyGLWidgetSpheres::mousePressEvent(QMouseEvent *e)
{
    makeCurrent();
    mousePos = glm::vec2(e->x(), resolution.y - e->y());
    update();
}

void MyGLWidgetSpheres::changeToRed(int value)
{
    makeCurrent();
    glClearColor((float)value / 100, 0.0, 0.0, 1.0);
    update();
}

void MyGLWidgetSpheres::showState(int s)
{
    std::cout << "Current State: " << s << std::endl;
}

void MyGLWidgetSpheres::sendLightToShader()
{
    glUniform3fv(lightPositionLoc, 1, glm::value_ptr(myLightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(myLightColor));
    glUniform3fv(globalAmbientLoc, 1, glm::value_ptr(myGlobalAmbient));
}

void MyGLWidgetSpheres::sendSpheresToShader()
{
    glUniform1i(numSpheresLoc, (int)mySpheres.size());

    for(size_t i = 0; i < mySpheres.size(); i++)
    {
        QString posStr = QString("spheres[%1].position").arg(i);
        QString colStr = QString("spheres[%1].color").arg(i);
        QString radStr = QString("spheres[%1].radius").arg(i);
        QString kaStr = QString("spheres[%1].ka").arg(i);
        QString kdStr = QString("spheres[%1].kd").arg(i);
        QString ksStr = QString("spheres[%1].ks").arg(i);
        QString shinStr = QString("spheres[%1].shininess").arg(i);

        GLuint posLoc = glGetUniformLocation(program->programId(), posStr.toStdString().c_str());
        GLuint colLoc = glGetUniformLocation(program->programId(), colStr.toStdString().c_str());
        GLuint radLoc = glGetUniformLocation(program->programId(), radStr.toStdString().c_str());
        GLuint kaLoc = glGetUniformLocation(program->programId(), kaStr.toStdString().c_str());
        GLuint kdLoc = glGetUniformLocation(program->programId(), kdStr.toStdString().c_str());
        GLuint ksLoc = glGetUniformLocation(program->programId(), ksStr.toStdString().c_str());

        glm::vec4 pos4(mySpheres[i].center, 1.0f);

        glUniform4fv(posLoc, 1, glm::value_ptr(pos4));
        glUniform4fv(colLoc, 1, glm::value_ptr(mySpheres[i].color));
        glUniform1f(radLoc, mySpheres[i].radius);
        glUniform3fv(kaLoc, 1, glm::value_ptr(mySpheres[i].ka));
        glUniform3fv(kdLoc, 1, glm::value_ptr(mySpheres[i].kd));
        glUniform3fv(ksLoc, 1, glm::value_ptr(mySpheres[i].ks));
    }
}

void MyGLWidgetSpheres::setCurrentCenterX(double x) { currentCenterTemp.x = (float)x; update(); }
void MyGLWidgetSpheres::setCurrentCenterY(double y) { currentCenterTemp.y = (float)y; update(); }
void MyGLWidgetSpheres::setCurrentCenterZ(double z) { currentCenterTemp.z = (float)z; update(); }
void MyGLWidgetSpheres::setCurrentRadius(int r)  { currentRadiusTemp = r / 100.0f; }

void MyGLWidgetSpheres::setCurrentColorR(int r) { currentColorTemp.r = r / 255.0f; }
void MyGLWidgetSpheres::setCurrentColorG(int g) { currentColorTemp.g = g / 255.0f; }
void MyGLWidgetSpheres::setCurrentColorB(int b) { currentColorTemp.b = b / 255.0f; }

void MyGLWidgetSpheres::setCurrentKaR(int r) { currentKaTemp.r = r / 100.0f; }
void MyGLWidgetSpheres::setCurrentKaG(int g) { currentKaTemp.g = g / 100.0f; }
void MyGLWidgetSpheres::setCurrentKaB(int b) { currentKaTemp.b = b / 100.0f; }

void MyGLWidgetSpheres::setCurrentKdR(int r) { currentKdTemp.r = r / 100.0f; }
void MyGLWidgetSpheres::setCurrentKdG(int g) { currentKdTemp.g = g / 100.0f; }
void MyGLWidgetSpheres::setCurrentKdB(int b) { currentKdTemp.b = b / 100.0f; }

void MyGLWidgetSpheres::setCurrentKsR(int r) { currentKsTemp.r = r / 100.0f; }
void MyGLWidgetSpheres::setCurrentKsG(int g) { currentKsTemp.g = g / 100.0f; }
void MyGLWidgetSpheres::setCurrentKsB(int b) { currentKsTemp.b = b / 100.0f; }

void MyGLWidgetSpheres::setCurrentShininess(int s) { currentShininessTemp = (float)s; update(); }

void MyGLWidgetSpheres::setLightPosX(int x) { myLightPos.x = (float)x; update(); }
void MyGLWidgetSpheres::setLightPosY(int y) { myLightPos.y = (float)y; update(); }
void MyGLWidgetSpheres::setLightPosZ(int z) { myLightPos.z = (float)z; update(); }

void MyGLWidgetSpheres::setLightColorR(int r) { myLightColor.r = r / 255.0f; update(); }
void MyGLWidgetSpheres::setLightColorG(int g) { myLightColor.g = g / 255.0f; update(); }
void MyGLWidgetSpheres::setLightColorB(int b) { myLightColor.b = b / 255.0f; update(); }

void MyGLWidgetSpheres::setGlobalAmbientR(int r) { myGlobalAmbient.r = r / 255.0f; update(); }
void MyGLWidgetSpheres::setGlobalAmbientG(int g) { myGlobalAmbient.g = g / 255.0f; update(); }
void MyGLWidgetSpheres::setGlobalAmbientB(int b) { myGlobalAmbient.b = b / 255.0f; update(); }

void MyGLWidgetSpheres::addSphere()
{
    if (mySpheres.size() >= 50) return;

    SphereData newSphere;
    newSphere.center = currentCenterTemp;
    newSphere.radius = currentRadiusTemp;
    newSphere.color = currentColorTemp;
    newSphere.ka = currentKaTemp;
    newSphere.kd = currentKdTemp;
    newSphere.ks = currentKsTemp;

    mySpheres.push_back(newSphere);
    update();
}
