#ifndef MYGLWIDGETSPHERES_H
#define MYGLWIDGETSPHERES_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Scene.h>


using namespace glm;

struct SphereData {
    vec3 center;
    float radius;
    vec4 color;
    vec3 ka;
    vec3 kd;
    vec3 ks;
};

class MyGLWidgetSpheres : public QOpenGLWidget, QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

    public:
        MyGLWidgetSpheres(QWidget *parent=0);
        ~MyGLWidgetSpheres();

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        virtual void mousePressEvent ( QMouseEvent *e);
        virtual void keyPressEvent ( QKeyEvent *e);

    private:
        Scene scene;

        void createBuffers();
        void loadShaders();
        void getShaderLocations();
        void sendSpheresToShader();
        void sendLightToShader();

        vec2 mousePos;
        vec2 resolution;

        GLuint VAO1;
        GLuint vertexLoc, scaleLoc, mousePosLoc  ;

        GLuint numSpheresLoc;
        GLuint lightPositionLoc, lightColorLoc, globalAmbientLoc;
        GLuint cameraPosLoc, globalShininessLoc;
        GLuint camForwardLoc, camRightLoc, camUpLoc;

        QOpenGLShaderProgram* program;

        std::vector<SphereData> mySpheres;

        vec3 myLightPos;
        vec3 myLightColor;
        vec3 myGlobalAmbient;

        glm::vec3 currentCenterTemp;
        float currentRadiusTemp;
        glm::vec4 currentColorTemp;
        glm::vec3 currentKaTemp, currentKdTemp, currentKsTemp;
        float currentShininessTemp;

    public slots:
        void changeToRed(int value);
        void showState(int s);

        void addSphere();

        void setCurrentCenterX(double x);
        void setCurrentCenterY(double y);
        void setCurrentCenterZ(double z);
        void setCurrentRadius(int r);
        
        void setCurrentColorR(int r);
        void setCurrentColorG(int g);
        void setCurrentColorB(int b);
        
        void setCurrentKaR(int r);
        void setCurrentKaG(int g);
        void setCurrentKaB(int b);
        
        void setCurrentKdR(int r);
        void setCurrentKdG(int g);
        void setCurrentKdB(int b);
        
        void setCurrentKsR(int r);
        void setCurrentKsG(int g);
        void setCurrentKsB(int b);

        void setCurrentShininess(int s);

        void setLightPosX(int x);
        void setLightPosY(int y);
        void setLightPosZ(int z);

        void setLightColorR(int r);
        void setLightColorG(int g);
        void setLightColorB(int b);

        void setGlobalAmbientR(int r);
        void setGlobalAmbientG(int g);
        void setGlobalAmbientB(int b);
};

#endif // MYGLWIDGETSPHERES_H
