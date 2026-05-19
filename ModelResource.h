#ifndef MODELRESOURCE_H
#define MODELRESOURCE_H

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QString>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.h"

class ModelResource : public QOpenGLFunctions_4_3_Core
{
public:
    ModelResource();
    ~ModelResource();

    void init(const QString& modelName, const QString& vertexShader, const QString& fragmentShader);
    void render(const glm::mat4& TG, const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, bool textureActive, bool bumpTextureActive, float attValue, const glm::vec4& clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, 100000.0f));
    void renderWater(const glm::mat4& TG, const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, GLuint reflectionTex, GLuint refractionTex, GLuint dudvTex, GLuint normalTex, float moveFactor, const glm::vec3& cameraPos);
    
    glm::vec3 getCenter() const;
    glm::vec3 getMin();
    glm::vec3 getMax();

private:
    void initTexture();
    void loadShaders(const QString& vShader, const QString& fShader);
    void createBuffers();
    void computeAABB();

    Model m;
    QOpenGLShaderProgram* program;

    GLuint VAO;
    GLuint VBO_vert, VBO_normals, VBO_matamb, VBO_matdiff, VBO_matspec, VBO_matshin;
    GLint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc, TGLoc, viewLoc, projLoc;

    float xmin, xmax, ymin, ymax, zmin, zmax;
    float aabb[6];

    GLuint textureID = 0;
    GLuint textureBumpID = 0;
    GLuint hasTextureLoc, hasBumpLoc, texCoordLoc, difuseTexLoc, bumpTextureLoc, tangentLoc, bitangentLoc;
    GLint attLoc;
};

#endif // MODELRESOURCE_H
