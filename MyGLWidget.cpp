#include "MyGLWidget.h"
#include <iostream>

MyGLWidget::MyGLWidget(QWidget* parent):QOpenGLWidget(parent)
{
}

MyGLWidget::~MyGLWidget()
{

}

void MyGLWidget::createBuffers ()
{
    /*glm::vec3 Vertices[3];  // Defines a triangle with default camera
    Vertices[0] = glm::vec3(-1.0, -1.0, 0.0);
    Vertices[1] = glm::vec3(1.0, -1.0, 0.0);
    Vertices[2] = glm::vec3(0.0, 1.0, 0.0);*/

    glm::vec3 VertCol[6];
    VertCol[0] = glm::vec3(-1.0, -1.0, 0.0);
    VertCol[1] = glm::vec3(1.0, 0.0, 0.0);
    VertCol[2] = glm::vec3(1.0, -1.0, 0.0);
    VertCol[3] = glm::vec3(0.0, 1.0, 0.0);
    VertCol[4] = glm::vec3(0.0, 1.0, 0.0);
    VertCol[5] = glm::vec3(0.0, 0.0, 1.0);

   // Creation of the Vertex Array Object (VAO)
   glGenVertexArrays(1, &VAO1);   //1. Generate VAO
   glBindVertexArray(VAO1);           //2. Bind VAO

   GLsizei stride = 2 * sizeof(glm::vec3);

   // Creation of the VBO with vertices data
   GLuint VBO;
   glGenBuffers(1, &VBO);             //3. Generate VBO
   glBindBuffer(GL_ARRAY_BUFFER, VBO);   //4. Activate the VBO
   glBufferData(GL_ARRAY_BUFFER, sizeof(VertCol), VertCol, GL_STATIC_DRAW); //5.Fill the VBO

   // Activation of the attribute
   glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);   //6. Activate attribute    3=number of components of the vertices
   glEnableVertexAttribArray(vertexLoc);


   /*glm::vec4 Colors[3]; // Defines the colors for the vertexs
   Colors[0] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
   Colors[1] = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
   Colors[2] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);*/

   /*/glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW); //5.Fill the VBO */

   // Activation of the attribute
   glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));   //6. Activate attribute    3=number of components of the vertices
   glEnableVertexAttribArray(colorLoc);

   // Disable the VAO
   glBindVertexArray(0);
}

void MyGLWidget::createBuffers2()
{
    glm::vec3 VertCol[4];
    VertCol[0] = glm::vec3(-1.0, 1.0, -1.0);
    VertCol[1] = glm::vec3(-1.0, -1.0, -1.0);
    VertCol[2] = glm::vec3(1.0, 1.0, -1.0);
    VertCol[3] = glm::vec3(1.0, -1.0, -1.0);

   // Creation of the Vertex Array Object (VAO)
   glGenVertexArrays(1, &VAO1);   //1. Generate VAO
   glBindVertexArray(VAO1);           //2. Bind VAO

   GLuint VBO;
   glGenBuffers(1, &VBO);             //3. Generate VBO
   glBindBuffer(GL_ARRAY_BUFFER, VBO);   //4. Activate the VBO
   glBufferData(GL_ARRAY_BUFFER, sizeof(VertCol), VertCol, GL_STATIC_DRAW); //5.Fill the VBO

   // Activation of the attribute
   glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);   //6. Activate attribute    3=number of components of the vertices
   glEnableVertexAttribArray(vertexLoc);

   glBindVertexArray(0);
}


void MyGLWidget::loadShaders()
{
    program=new QOpenGLShaderProgram(this);
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

    program->bind();
    getShaderLocations();
}

void MyGLWidget::getShaderLocations()
{
    vertexLoc = glGetAttribLocation(program->programId(), "vertex");
    colorLoc = glGetAttribLocation(program->programId(), "color");
    scaleLoc = glGetUniformLocation(program->programId(), "screenSize");
    idShaderLoc = glGetUniformLocation(program->programId(), "idShader");
    halfLoc = glGetUniformLocation(program->programId(), "halfScreen");
    mousePosLoc = glGetUniformLocation(program->programId(), "mousePosition");
    resolutionLoc = glGetUniformLocation(program->programId(), "resolution");

    numSpheresLoc = glGetUniformLocation(program->programId(), "numSpheres");
    lightPositionLoc = glGetUniformLocation(program->programId(), "lightPosition");
    lightColorLoc = glGetUniformLocation(program->programId(), "lightColor");
    globalAmbientLoc = glGetUniformLocation(program->programId(), "globalAmbient");

}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.76, 0.69, 0.52, 1.0);
    loadShaders();

    scene.init();
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    program->bind();

    sendLightToShader();
    sendSpheresToShader();

    glm::mat4 viewMat = scene.getCamera().getViewMatrix();
    glm::mat4 projMat = scene.getCamera().getProjectMatrix();

    GLuint viewLoc = glGetUniformLocation(program->programId(), "view");
    GLuint projLoc = glGetUniformLocation(program->programId(), "proj");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &viewMat[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projMat[0][0]);

    scene.render();

    program->release();
}

void MyGLWidget::resizeGL(int width, int height)
{
    glm::vec2 screenSize(width, height);
    glUniform2fv(scaleLoc, 1, &screenSize[0]);

    resolution = glm::vec2(width, height);
    glUniform2fv(resolutionLoc, 1, &resolution[0]);
    halfVP = width / 2;
    glUniform1f(halfLoc, halfVP);

    float ratio = (float)width / (float)height;
    scene.getCamera().setAspectRatio(ratio);
}

void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    makeCurrent();
    float speed = 0.1f;
    float angle = glm::radians(1.0f);

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
            //scene.getCamera().init();
            break;
        default:
            e->ignore();
            return;
         }

     update();
}

void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
    makeCurrent();
    mousePos = glm::vec2(e->x(), resolution.y - e->y());
    glUniform2fv(mousePosLoc, 1, &mousePos[0]);
    update();
}

void MyGLWidget::changeToRed(int value)
{
    makeCurrent();
    glClearColor((float)value / 100, 0.0, 0.0, 1.0);
    update();
}

void MyGLWidget::changeToButton1() { makeCurrent(); glUniform1i(idShaderLoc, 1); update(); }
void MyGLWidget::changeToButton2() { makeCurrent(); glUniform1i(idShaderLoc, 2); update(); }
void MyGLWidget::changeToButton3() { makeCurrent(); glUniform1i(idShaderLoc, 3); update(); }
void MyGLWidget::changeToButton4() { makeCurrent(); glUniform1i(idShaderLoc, 4); update(); }

void MyGLWidget::showState(int s)
{
    std::cout << "Current State: " << s << std::endl;
}

void MyGLWidget::sendLightToShader()
{
    glUniform3fv(lightPositionLoc, 1, glm::value_ptr(myLightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(myLightColor));
    glUniform3fv(globalAmbientLoc, 1, glm::value_ptr(myGlobalAmbient));
}

void MyGLWidget::sendSpheresToShader()
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

void MyGLWidget::setCurrentCenterX(double x) { currentCenterTemp.x = (float)x; }
void MyGLWidget::setCurrentCenterY(double y) { currentCenterTemp.y = (float)y; }
void MyGLWidget::setCurrentCenterZ(double z) { currentCenterTemp.z = (float)z; }
void MyGLWidget::setCurrentRadius(double r)  { currentRadiusTemp = (float)r; }

void MyGLWidget::setCurrentColorR(int r) { currentColorTemp.r = r / 100.0f; }
void MyGLWidget::setCurrentColorG(int g) { currentColorTemp.g = g / 100.0f; }
void MyGLWidget::setCurrentColorB(int b) { currentColorTemp.b = b / 100.0f; }

void MyGLWidget::setCurrentKaR(double r) { currentKaTemp.r = (float)r; }
void MyGLWidget::setCurrentKaG(double g) { currentKaTemp.g = (float)g; }
void MyGLWidget::setCurrentKaB(double b) { currentKaTemp.b = (float)b; }

void MyGLWidget::setCurrentKdR(double r) { currentKdTemp.r = (float)r; }
void MyGLWidget::setCurrentKdG(double g) { currentKdTemp.g = (float)g; }
void MyGLWidget::setCurrentKdB(double b) { currentKdTemp.b = (float)b; }

void MyGLWidget::setCurrentKsR(double r) { currentKsTemp.r = (float)r; }
void MyGLWidget::setCurrentKsG(double g) { currentKsTemp.g = (float)g; }
void MyGLWidget::setCurrentKsB(double b) { currentKsTemp.b = (float)b; }

void MyGLWidget::setLightPosX(double x) { myLightPos.x = x; update(); }
void MyGLWidget::setLightPosY(double y) { myLightPos.y = y; update(); }
void MyGLWidget::setLightPosZ(double z) { myLightPos.z = z; update(); }

void MyGLWidget::setLightColorR(double r) { myLightColor.r = r; update(); }
void MyGLWidget::setLightColorG(double g) { myLightColor.g = g; update(); }
void MyGLWidget::setLightColorB(double b) { myLightColor.b = b; update(); }

void MyGLWidget::setGlobalAmbientR(double r) { myGlobalAmbient.r = r; update(); }
void MyGLWidget::setGlobalAmbientG(double g) { myGlobalAmbient.g = g; update(); }
void MyGLWidget::setGlobalAmbientB(double b) { myGlobalAmbient.b = b; update(); }

void MyGLWidget::addSphere()
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
