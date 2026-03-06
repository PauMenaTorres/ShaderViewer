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

void MyGLWidget::loadShaders()
{
    program=new QOpenGLShaderProgram(this);
    if (!QFile::exists(":/fragment.frag")) {

        qCritical() << "ERROR: El fitxer :/fragment.frag no existeix als recursos!";
        return;
    }

    if (!QFile::exists(":/vertex.vert")) {

        qCritical() << "ERROR: El fitxer :/vertex.vert no existeix als recursos!";
        return;
    }

    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vertex.vert")) {

        qCritical() << "Error en Vertex Shader:" << program->log();

    }

    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fragment.frag")) {

        qCritical() << "Error en Fragment Shader:" << program->log();

    }

    if (!program->link()) {

        qCritical() << "Error enllaçant el programa:" << program->log();

    }

    program->bind();

    vertexLoc=glGetAttribLocation(program->programId(), "vertex");
    colorLoc=glGetAttribLocation(program->programId(), "color");
    scaleLoc=glGetUniformLocation(program->programId(), "screenSize");
    idShaderLoc=glGetUniformLocation(program->programId(), "idShader");
    halfLoc=glGetUniformLocation(program->programId(), "halfScreen");
    mousePosLoc=glGetUniformLocation(program->programId(), "mousePosition");
}

void MyGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.5 , 0.7, 1.0, 1.0);
    loadShaders();
    createBuffers();
    glUniform1i(idShaderLoc, 1);

//    GLint vp[4];
//    glGetIntegerv(GL_VIEWPORT, vp);
//    halfVP = vp[2];
//    glUniform1f(halfLoc, halfVP);
}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(VAO1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void MyGLWidget::resizeGL(int width, int height)
{
    glm::vec2 screenSize(width, height);
    glUniform2fv(scaleLoc, 1, &screenSize[0]);

    resolution = glm::vec2(width, height);
    glUniform2fv(resolutionLoc, 1, &resolution[0]);
    halfVP = width / 2;
    glUniform1f(halfLoc, halfVP);
}

void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    makeCurrent ();   //Activate the OpenGL context
    switch ( e->key() )
    {
        case  Qt::Key_A :
            halfVP--;
            glUniform1f(halfLoc, halfVP);
            break;
        case  Qt::Key_D :
            halfVP++;
            glUniform1f(halfLoc, halfVP);
            break;
        default: e->ignore (); // propagate to parent
     }
     update ();  //Refresh window
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
    makeCurrent(); // Hace que el contexto gráfico sea el actual
    glClearColor((float)value / 100, 0.0, 0.0, 1.0);
    update();
}


void MyGLWidget::changeToButton1()
{
    makeCurrent();
    glUniform1i(idShaderLoc, 1);
    update();
}


void MyGLWidget::changeToButton2()
{
    makeCurrent();
    glUniform1i(idShaderLoc, 2);
    update();
}

void MyGLWidget::changeToButton3()
{
    makeCurrent();
    glUniform1i(idShaderLoc, 3);
    update();
}


void MyGLWidget::changeToButton4()
{
    makeCurrent();
    glUniform1i(idShaderLoc, 4);
    update();
}

void MyGLWidget::showState(int s)
{
    std::cout << "Current State: " << s << std::endl;
}
