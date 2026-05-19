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
    if (waterFbos) {
        delete waterFbos;
    }
}


void MyGLWidgetScene::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.25f, 0.61f, 0.9f, 1.0f);
    
    scene.loadWaterScene();

    int w = width() * devicePixelRatio();
    int h = height() * devicePixelRatio();
    waterFbos = new WaterFrameBuffers(w, h);

    initGBuffer();
    initQuad();
    initWaterTextures();

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
    int w = width() * devicePixelRatio();
    int h = height() * devicePixelRatio();

    // Increment moveFactor to animate waves using dynamic speed
    moveFactor += myWaveSpeed;
    if (moveFactor >= 1.0f) moveFactor -= 1.0f;

    // 1. Render Reflection (Y > 0.0f)
    waterFbos->bindReflectionFrameBuffer();
    glClearColor(0.25f, 0.61f, 0.9f, 1.0f); // Beautiful sky blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate reflected view matrix by negating the Y-scaling column of the normal view matrix (reflection across Y = 0)
    glm::mat4 normalViewMat = scene.getCamera().getViewMatrix();
    glm::mat4 reflectionViewMat = normalViewMat;
    reflectionViewMat[1] = -normalViewMat[1]; // Column 1 corresponds to Y scaling in GLM (column-major)
    glm::mat4 projMat = scene.getCamera().getProjectMatrix();

    // Render all models from the reflected view, clipping everything below water (Y < 0), with reflection/refraction lighting enabled
    for (ModelInstance* inst : scene.getInstances()) {
        inst->render(reflectionViewMat, projMat, myLightPos, myLightColor, myAtt, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), true);
    }
    
    // 2. Render Refraction (Y < 0.0f)
    waterFbos->bindRefractionFrameBuffer();
    glClearColor(0.25f, 0.61f, 0.9f, 1.0f); // Consistent sky blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render all models from normal view, clipping everything above water (Y > 0), with reflection/refraction lighting enabled
    for (ModelInstance* inst : scene.getInstances()) {
        inst->render(scene.getCamera().getViewMatrix(), projMat, myLightPos, myLightColor, myAtt, glm::vec4(0.0f, -1.0f, 0.0f, 0.0f), true);
    }

    // 3. Geometry Pass normal: render scene data to G-Buffer
    waterFbos->unbindCurrentFrameBuffer(w, h);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Clear G-Buffer with zero (normals will have length 0 on background)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render models normally without clipping
    scene.renderModelsOnly(myLightPos, myLightColor, myAtt, glm::vec4(0.0f, 1.0f, 0.0f, 100000.0f));
    
    // Render water with reflection/refraction and procedural DuDv/Normal maps
    scene.renderWaterOnly(myLightPos, myLightColor, myAtt, 
                          waterFbos->getReflectionTexture(), 
                          waterFbos->getRefractionTexture(), 
                          dudvTexture, 
                          normalTexture, 
                          moveFactor,
                          myWaveStrength,
                          myWaterShininess);
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // 4. Lighting Pass: calculate lighting using G-Buffer textures
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

    // Trigger next frame update for continuous wave animation
    update();
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

void MyGLWidgetScene::initWaterTextures()
{
    makeCurrent();

    const int width = 256;
    const int height = 256;
    const float pi = 3.14159265f;

    // Generate DuDv Map (distortion map)
    std::vector<unsigned char> dudvData(width * height * 4); // RGBA
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float fx = (float)x / width * 2.0f * pi * 4.0f; // frequency
            float fy = (float)y / height * 2.0f * pi * 4.0f;
            
            float du = sin(fx + fy) * cos(fy) * 0.5f + 0.5f;
            float dv = cos(fx - fy) * sin(fx) * 0.5f + 0.5f;
            
            int idx = (y * width + x) * 4;
            dudvData[idx + 0] = (unsigned char)(du * 255.0f); // R -> X offset
            dudvData[idx + 1] = (unsigned char)(dv * 255.0f); // G -> Y offset
            dudvData[idx + 2] = 0;                            // B
            dudvData[idx + 3] = 255;                          // A
        }
    }

    glGenTextures(1, &dudvTexture);
    glBindTexture(GL_TEXTURE_2D, dudvTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dudvData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Generate Normal Map (tangent space)
    std::vector<unsigned char> normalData(width * height * 4); // RGBA
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto getHeight = [](float px, float py) {
                float h = 0.0f;
                h += sin(px * 0.15f + py * 0.08f) * 1.2f;
                h += cos(px * 0.10f - py * 0.14f) * 0.8f;
                h += sin(px * 0.25f + py * 0.20f) * 0.4f;
                return h;
            };

            float h_center = getHeight(x, y);
            float h_r = getHeight(x + 1, y);
            float h_u = getHeight(x, y + 1);

            float dh_dx = h_r - h_center;
            float dh_dy = h_u - h_center;

            glm::vec3 normal = glm::normalize(glm::vec3(-dh_dx * 2.5f, 1.0f, -dh_dy * 2.5f));

            int idx = (y * width + x) * 4;
            normalData[idx + 0] = (unsigned char)((normal.x * 0.5f + 0.5f) * 255.0f);
            normalData[idx + 1] = (unsigned char)((normal.z * 0.5f + 0.5f) * 255.0f);
            normalData[idx + 2] = (unsigned char)((normal.y * 0.5f + 0.5f) * 255.0f);
            normalData[idx + 3] = 255;
        }
    }

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalData.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void MyGLWidgetScene::setWaveSpeed(int speed)
{
    myWaveSpeed = (float)speed / 10000.0f;
    update();
}

void MyGLWidgetScene::setWaveStrength(int strength)
{
    myWaveStrength = (float)strength / 1000.0f;
    update();
}

void MyGLWidgetScene::setReflectivity(int reflectivity)
{
    myWaterShininess = (float)reflectivity;
    update();
}
