#ifndef MyGLWidgetScene_H
#define MyGLWidgetScene_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QKeyEvent>
#include <QVector3D>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <model.h>
#include <Scene.h>
#include "WaterFrameBuffers.h"

using namespace glm;

class MyGLWidgetScene : public QOpenGLWidget, QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

    public:
        MyGLWidgetScene(QWidget *parent=0);
        ~MyGLWidgetScene();

protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        virtual void keyPressEvent(QKeyEvent *e);

    private:
        Scene scene;

        float halfVP;
        vec2 resolution;

        // G-Buffer
        GLuint gBuffer;
        GLuint gPosition, gNormal, gAlbedoSpec;
        GLuint rboDepth;
        void initGBuffer();
        
        WaterFrameBuffers* waterFbos = nullptr;

        GLuint dudvTexture = 0;
        GLuint normalTexture = 0;
        float moveFactor = 0.0f;
        void initWaterTextures();

        // Lighting Pass
        QOpenGLShaderProgram *lightingShader;
        GLuint quadVAO, quadVBO;
        void initQuad();
        void renderQuad();

        vec3 myLightPos;
        vec3 myLightColor;
        float myAtt = 1.0f;

        float myWaveSpeed = 0.0002f;
        float myWaveStrength = 0.01f;
        float myWaterShininess = 16.0f;

    public slots:
        void setLightPosX(int x);
        void setLightPosY(int y);
        void setLightPosZ(int z);

        void setLightColorR(int r);
        void setLightColorG(int g);
        void setLightColorB(int b);

        void setAtt(int value);

        void activeTexture(bool isTextureActive);
        void activeBump(bool isBumpActive);

        void setWaveSpeed(int speed);
        void setWaveStrength(int strength);
        void setReflectivity(int reflectivity);
};

#endif // MyGLWidgetScene_H
