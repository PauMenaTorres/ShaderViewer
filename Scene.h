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
    void update();
    void render(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue);

    Camera& getCamera();

    std::vector<ModelInstance*> getInstances() const;

    ModelResource* getResource(const QString& path);

private:

    std::map<QString, ModelResource*> resources;
    std::vector<ModelInstance*> instances;
    Camera camera;
};

#endif // SCENE_H
