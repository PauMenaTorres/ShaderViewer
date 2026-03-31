#ifndef MODELOBJ_H
#define MODELOBJ_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QString>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.h"

class ModelOBJ : public QOpenGLFunctions_4_3_Core
{
public:
    ModelOBJ();
    ~ModelOBJ();

    void init(const QString& modelName, const QString& vertexShader, const QString& fragmentShader);
    void modelTransform(const glm::mat4& transform);
    void render();

private:

    Model m;
    glm::mat4 TG;
    QOpenGLShaderProgram* program;

    GLuint VAO;
    GLuint VBO_vert, VBO_mat;
    GLint vertexLoc, colorLoc, TGLoc;

    float aabb[6];

    void loadShaders(const QString& vShader, const QString& fShader);
    void createBuffers();
    void computeAABB();
};

#endif // MODELOBJ_H
