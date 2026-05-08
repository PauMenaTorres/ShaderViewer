#include "Scene.h"

Scene::Scene()
{
}

Scene::~Scene()
{
    for (ModelInstance* inst : instances) {
        delete inst;
    }
    instances.clear();
    
    for (auto const& pair : resources) {
        delete pair.second;
    }
    resources.clear();
}

ModelResource* Scene::getResource(const QString& path)
{
    if (resources.find(path) == resources.end()) {
        ModelResource* res = new ModelResource();
        res->init(path, ":/vertexModel.vert", ":/fragmentModel.frag");
        resources[path] = res;
    }
    return resources[path];
}

void Scene::init()
{
    for(ModelInstance* inst : instances)
    {
        delete inst;
    }
    instances.clear();
}

void Scene::loadModel(const QString& path)
{
    init();

    ModelResource* res = getResource(path);
    ModelInstance* inst = new ModelInstance(res);
    
    glm::mat4 tg(1.0f);
    glm::vec3 center = res->getCenter();
    tg = glm::translate(tg, -center);
    inst->modelTransform(tg);

    instances.push_back(inst);

    glm::vec3 boxMin = res->getMin() - center;
    glm::vec3 boxMax = res->getMax() - center;
    camera.init(boxMin, boxMax, true);
}

void Scene::loadScene()
{
    init();

    ModelResource* wallRes = getResource("Models3D/graf_wall.obj");
    glm::vec3 wallCenter = wallRes->getCenter();

    glm::vec3 wallMin = wallRes->getMin();
    glm::vec3 wallMax = wallRes->getMax();

    float alturaActualWall = wallMax.y - wallMin.y;

    float alturaDeseadaWall = 3.0f;
    float scaleFactorWall = alturaDeseadaWall / alturaActualWall;

    ModelResource* farolaRes = getResource("Models3D/farola1.obj");
    glm::vec3 farolaCenter = farolaRes->getCenter();

    glm::vec3 farolaMin = farolaRes->getMin();
    glm::vec3 farolaMax = farolaRes->getMax();

    float alturaActualFarola = farolaMax.y - farolaMin.y;

    float alturaDeseadaFarola = 1.5f;
    float scaleFactorFarola = alturaDeseadaFarola / alturaActualFarola;

    // --- Pared: centrada en (0,0,0), altura 3 ---
    ModelInstance* wallInst = new ModelInstance(wallRes);
    glm::mat4 tgWall(1.0f);
    tgWall = glm::scale(tgWall, glm::vec3(scaleFactorWall));
    tgWall = glm::rotate(tgWall, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    tgWall = glm::translate(tgWall, -wallCenter);
    wallInst->modelTransform(tgWall);
    instances.push_back(wallInst);

    // --- Farola 1: posición (-0.5, 0, 3.5) ---
    ModelInstance* farolaInst1 = new ModelInstance(farolaRes);
    glm::mat4 tgFarola1(1.0f);
    tgFarola1 = glm::rotate(tgFarola1, glm::radians(-90.0f), glm::vec3(0,1,0));
    tgFarola1 = glm::translate(tgFarola1, glm::vec3(-0.5f, 0.0f, 3.5f));
    tgFarola1 = glm::scale(tgFarola1, glm::vec3(scaleFactorFarola));
    tgFarola1 = glm::translate(tgFarola1, -farolaCenter);
    farolaInst1->modelTransform(tgFarola1);
    instances.push_back(farolaInst1);

    // --- Farola 2: posición (-0.5, 0, 1.5) ---
    ModelInstance* farolaInst2 = new ModelInstance(farolaRes);
    glm::mat4 tgFarola2(1.0f);
    tgFarola2 = glm::rotate(tgFarola2, glm::radians(-90.0f), glm::vec3(0,1,0));
    tgFarola2 = glm::translate(tgFarola2, glm::vec3(-0.5f, 0.0f, 1.5f));
    tgFarola2 = glm::scale(tgFarola2, glm::vec3(scaleFactorFarola));
    tgFarola2 = glm::translate(tgFarola2, -farolaCenter);
    farolaInst2->modelTransform(tgFarola2);
    instances.push_back(farolaInst2);

    // --- Farola 3: posición (-0.5, 0, -0.5) ---
    ModelInstance* farolaInst3 = new ModelInstance(farolaRes);
    glm::mat4 tgFarola3(1.0f);
    tgFarola3 = glm::rotate(tgFarola3, glm::radians(-90.0f), glm::vec3(0,1,0));
    tgFarola3 = glm::translate(tgFarola3, glm::vec3(-0.5f, 0.0f, -0.5f));
    tgFarola3 = glm::scale(tgFarola3, glm::vec3(scaleFactorFarola));
    tgFarola3 = glm::translate(tgFarola3, -farolaCenter);
    farolaInst3->modelTransform(tgFarola3);
    instances.push_back(farolaInst3);

    // --- Cámara: bounding box global que envuelve toda la escena ---
    glm::vec3 sceneMin = glm::vec3(-2.0f, -1.5f, -1.0f);
    glm::vec3 sceneMax = glm::vec3(2.0f, 2.0f, 4.5f);
    camera.init(sceneMin, sceneMax, true);
}

void Scene::update()
{
}

void Scene::render(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue)
{
    for (ModelInstance* inst : instances) {
        inst->render(camera.getViewMatrix(), camera.getProjectMatrix(), lightPos, lightColor, attValue);
    }
}

Camera& Scene::getCamera()
{
    return camera;
}

std::vector<ModelInstance*> Scene::getInstances() const
{
    return instances;
}
