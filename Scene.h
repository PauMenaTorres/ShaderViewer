#ifndef SCENE_H
#define SCENE_H

#include <ModelOBJ.h>
#include <Camera.h>

class Scene
{
public:

    Scene();

    void init();
    void update(float dt);
    void render(const glm::vec3& lightPos, const glm::vec3& lightColor);

    Camera& getCamera();

    std::vector<ModelOBJ*> getModels() const;

private:

    std::vector<ModelOBJ*> models;
    Camera camera;
};

#endif // SCENE_H
