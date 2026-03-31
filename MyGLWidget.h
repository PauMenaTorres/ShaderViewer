#ifndef MYGLWIDGET_H

#define MYGLWIDGET_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <model.h>
#include <ModelOBJ.h>

using namespace glm;

struct SphereData {
    vec3 center;
    float radius;
    vec4 color;
    vec3 ka;
    vec3 kd;
    vec3 ks;
};

class MyGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_3_Core
{
    Q_OBJECT

    public:
        MyGLWidget(QWidget *parent=0);
        ~MyGLWidget();

    protected:
        void initializeGL();
        void paintGL();
        void resizeGL(int width, int height);

        virtual void mousePressEvent ( QMouseEvent *e);
        //virtual void mouseReleaseEvent ( QMouseEvent *e);
        //virtual void mouseMoveEvent ( QMouseEvent *e);
        virtual void keyPressEvent ( QKeyEvent *e);

        void modelTransform();

    private:

        ModelOBJ homer;
        ModelOBJ tree;

        void createBuffers();
        void createBuffers2();
        void loadShaders();

        void getShaderLocations();
        void sendSpheresToShader();
        void sendLightToShader();

        float halfVP;
        vec2 mousePos;
        vec2 resolution;

        GLuint VAO1;
        GLuint vertexLoc, colorLoc, scaleLoc, idShaderLoc, halfLoc, resolutionLoc, mousePosLoc  ;

        GLuint numSpheresLoc;
        GLuint lightPositionLoc, lightColorLoc, globalAmbientLoc;

        QOpenGLShaderProgram* program;

        std::vector<SphereData> mySpheres;

        vec3 myLightPos;
        vec3 myLightColor;
        vec3 myGlobalAmbient;

        glm::vec3 currentCenterTemp;
        float currentRadiusTemp;
        glm::vec4 currentColorTemp;
        glm::vec3 currentKaTemp, currentKdTemp, currentKsTemp;

    public slots:
        void changeToRed(int value);
        void showState(int s);
        void changeToButton1();
        void changeToButton2();
        void changeToButton3();
        void changeToButton4();

        void addSphere();

        void setCurrentCenterX(double x);
        void setCurrentCenterY(double y);
        void setCurrentCenterZ(double z);
        void setCurrentRadius(double r);
        void setCurrentColorR(int r);
        void setCurrentColorG(int g);
        void setCurrentColorB(int b);
        void setCurrentKaR(double r);
        void setCurrentKaG(double g);
        void setCurrentKaB(double b);
        void setCurrentKdR(double r);
        void setCurrentKdG(double g);
        void setCurrentKdB(double b);
        void setCurrentKsR(double r);
        void setCurrentKsG(double g);
        void setCurrentKsB(double b);

        void setLightPosX(double x);
        void setLightPosY(double y);
        void setLightPosZ(double z);

        void setLightColorR(double r);
        void setLightColorG(double g);
        void setLightColorB(double b);

        void setGlobalAmbientR(double r);
        void setGlobalAmbientG(double g);
        void setGlobalAmbientB(double b);
};

#endif // MYGLWIDGET_H
