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

ModelResource* Scene::getResource(const QString& path, const QString& fragShader)
{
    QString key = path + fragShader;
    if (resources.find(key) == resources.end()) {
        ModelResource* res = new ModelResource();
        res->init(path, ":/vertexModel.vert", fragShader);
        resources[key] = res;
    }
    return resources[key];
}

glm::mat4 Scene::createModelTransform(ModelResource* res, const glm::vec3& position, float targetHeight)
{
    glm::vec3 center = res->getCenter();
    glm::vec3 minBB  = res->getMin();
    glm::vec3 maxBB  = res->getMax();

    float currentHeight = maxBB.y - minBB.y;
    float scaleFactor   = targetHeight / currentHeight;

    glm::mat4 tg(1.0f);
    tg = glm::translate(tg, position);       // 3. colocar en posición final
    tg = glm::scale(tg, glm::vec3(scaleFactor)); // 2. escalar a la altura deseada
    tg = glm::translate(tg, -center);        // 1. centrar el objeto en el origen
    return tg;
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

    ModelResource* wallRes   = getResource("Models3D/graf_wall.obj", ":/geometryPass.frag");
    ModelResource* farolaRes = getResource("Models3D/farola1.obj",   ":/geometryPass.frag");

    // --- Pared: centrada en (0,0,0), altura 3 ---
    ModelInstance* wallInst = new ModelInstance(wallRes);
    wallInst->modelTransform(createModelTransform(wallRes, glm::vec3(0.0f), 3.0f));
    instances.push_back(wallInst);

    // --- Farola 1: posición (-0.5, 0, 3.5) ---
    ModelInstance* farolaInst1 = new ModelInstance(farolaRes);
    farolaInst1->modelTransform(createModelTransform(farolaRes, glm::vec3(-0.5f, 0.0f, 3.5f), 1.5f));
    instances.push_back(farolaInst1);

    // --- Farola 2: posición (-0.5, 0, 1.5) ---
    ModelInstance* farolaInst2 = new ModelInstance(farolaRes);
    farolaInst2->modelTransform(createModelTransform(farolaRes, glm::vec3(-0.5f, 0.0f, 1.5f), 1.5f));
    instances.push_back(farolaInst2);

    // --- Farola 3: posición (-0.5, 0, -0.5) ---
    ModelInstance* farolaInst3 = new ModelInstance(farolaRes);
    farolaInst3->modelTransform(createModelTransform(farolaRes, glm::vec3(-0.5f, 0.0f, -0.5f), 1.5f));
    instances.push_back(farolaInst3);

    // --- Cámara: bounding box global que envuelve toda la escena ---
    glm::vec3 sceneMin = glm::vec3(-2.0f, -1.5f, -1.0f);
    glm::vec3 sceneMax = glm::vec3(2.0f, 2.0f, 4.5f);
    camera.init(sceneMin, sceneMax, true);
    camera.orbitY(glm::radians(90.0f));
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
