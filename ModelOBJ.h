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
    void render(const glm::mat4& viewMat, const glm::mat4& projMat);
    glm::vec3 getCenter() const;

private:

    Model m;
    glm::mat4 TG = glm::mat4(1.0f);
    QOpenGLShaderProgram* program;

    GLuint VAO;
    GLuint VBO_vert, VBO_normals, VBO_matamb, VBO_matdiff, VBO_matspec, VBO_matshin;
    GLint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc, TGLoc, viewLoc, projLoc;

    float aabb[6];

    void loadShaders(const QString& vShader, const QString& fShader);
    void createBuffers();
    void computeAABB();
};

#endif // MODELOBJ_H
