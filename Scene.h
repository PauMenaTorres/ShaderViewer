#ifndef SCENE_H
#define SCENE_H

#include "ModelResource.h"
#include "ModelInstance.h"
#include "Camera.h"
#include <vector>
#include <map>
#include <QString>

class Scene
{
public:

    Scene();
    ~Scene();

    void init();
    void loadModel(const QString& path);
    void loadScene();
    void loadWaterScene();
    void update();
    void render(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue);
    void renderModelsOnly(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, const glm::vec4& clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, 100000.0f));
    void renderWaterOnly(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, GLuint reflectionTex = 0, GLuint refractionTex = 0, GLuint dudvTex = 0, GLuint normalTex = 0, float moveFactor = 0.0f, float waveStrength = 0.02f, float waterShininess = 32.0f);

    Camera& getCamera();

    std::vector<ModelInstance*> getInstances() const;

    ModelResource* getResource(const QString& path, const QString& fragShader = ":/geometryPass.frag");
    ModelResource* getResource(const QString& path, const QString& vertShader, const QString& fragShader);

private:

    glm::mat4 createModelTransform(ModelResource* res, const glm::vec3& position, float targetHeight);

    std::map<QString, ModelResource*> resources;
    std::vector<ModelInstance*> instances;
    ModelInstance* waterInst = nullptr;
    Camera camera;
};

#endif // SCENE_H
