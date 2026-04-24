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
    void render();

    Camera& getCamera();

    std::vector<ModelOBJ*> getModels() const;

private:

    std::vector<ModelOBJ*> models;
    Camera camera;
};

#endif // SCENE_H
