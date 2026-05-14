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
    glClearColor(0, 0, 0, 0);
    scene.loadWaterScene();

    initGBuffer();
    initQuad();

    lightingShader = new QOpenGLShaderProgram();
    lightingShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingPass.vert");
    lightingShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingPass.frag");
    lightingShader->link();

    lightingShader->bind();
    lightingShader->setUniformValue("gPosition", 0);
    lightingShader->setUniformValue("gNormal", 1);
    lightingShader->setUniformValue("gAlbedoSpec", 2);
    lightingShader->release();
}

void MyGLWidgetScene::paintGL()
{
    // 1. Geometry Pass: render scene data to G-Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    scene.render(myLightPos, myLightColor, myAtt);
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // 2. Lighting Pass: calculate lighting using G-Buffer textures
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightingShader->bind();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    
    // Transform lightPos to SCO for the shader
    glm::vec3 lightPosSCO = glm::vec3(scene.getCamera().getViewMatrix() * glm::vec4(myLightPos, 1.0f));
    lightingShader->setUniformValue("lightPos", QVector3D(lightPosSCO.x, lightPosSCO.y, lightPosSCO.z));
    lightingShader->setUniformValue("lightColor", QVector3D(myLightColor.r, myLightColor.g, myLightColor.b));
    lightingShader->setUniformValue("att", myAtt);

    renderQuad();
    
    lightingShader->release();
}

void MyGLWidgetScene::resizeGL(int width, int height)
{
    float ratio = (float)width / (float)height;
    scene.getCamera().setAspectRatio(ratio);

    // Re-initialize G-Buffer textures with new size
    initGBuffer();
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
void MyGLWidgetScene::initGBuffer()
{
    if (glIsFramebuffer(gBuffer)) {
        glDeleteFramebuffers(1, &gBuffer);
        glDeleteTextures(1, &gPosition);
        glDeleteTextures(1, &gNormal);
        glDeleteTextures(1, &gAlbedoSpec);
        glDeleteRenderbuffers(1, &rboDepth);
    }

    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    int w = width() * devicePixelRatio();
    int h = height() * devicePixelRatio();

    // Position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // Color + Specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyGLWidgetScene::initQuad()
{
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void MyGLWidgetScene::renderQuad()
{
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
