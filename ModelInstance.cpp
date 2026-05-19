#include "ModelInstance.h"

ModelInstance::ModelInstance(ModelResource* res)
    : resource(res), TG(1.0f), textureActive(true), bumpTextureActive(true)
{
}

ModelInstance::~ModelInstance()
{
}

void ModelInstance::modelTransform(const glm::mat4& transform)
{
    TG = transform;
}

void ModelInstance::render(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, const glm::vec4& clipPlane, bool isReflectionOrRefraction)
{
    if (resource) {
        resource->render(TG, viewMat, projMat, lightPos, lightColor, textureActive, bumpTextureActive, attValue, clipPlane, isReflectionOrRefraction);
    }
}

void ModelInstance::renderWater(const glm::mat4& viewMat, const glm::mat4& projMat, const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, GLuint reflectionTex, GLuint refractionTex, GLuint dudvTex, GLuint normalTex, float moveFactor, const glm::vec3& cameraPos, float waveStrength, float waterShininess)
{
    if (resource) {
        resource->renderWater(TG, viewMat, projMat, lightPos, lightColor, attValue, reflectionTex, refractionTex, dudvTex, normalTex, moveFactor, cameraPos, waveStrength, waterShininess);
    }
}

void ModelInstance::setTextureActive(bool active)
{
    textureActive = active;
}

void ModelInstance::setBumpActive(bool active)
{
    bumpTextureActive = active;
}

ModelResource* ModelInstance::getResource() const
{
    return resource;
}

glm::mat4 ModelInstance::getTransform() const
{
    return TG;
}
