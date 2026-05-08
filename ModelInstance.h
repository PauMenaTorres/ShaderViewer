#ifndef MODELINSTANCE_H
#define MODELINSTANCE_H

#include "ModelResource.h"
#include <glm/glm.hpp>

class ModelInstance
{
public:
    ModelInstance(ModelResource* resource);
    ~ModelInstance();

    void modelTransform(const glm::mat4& transform);
    void render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue);

    void setTextureActive(bool active);
    void setBumpActive(bool active);

    ModelResource* getResource() const;
    glm::mat4 getTransform() const;

private:
    ModelResource* resource;
    glm::mat4 TG;
    bool textureActive;
    bool bumpTextureActive;
};

#endif // MODELINSTANCE_H
