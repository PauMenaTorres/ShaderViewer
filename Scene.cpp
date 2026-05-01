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

    ModelResource* treeRes = getResource("Models3D/tree.obj");
    ModelResource* catRes = getResource("Models3D/cat.obj");
    ModelResource* patricioRes = getResource("Models3D/Patricio.obj");

    ModelInstance* catInst = new ModelInstance(catRes);
    glm::mat4 tgCat(1.0f);
    tgCat = glm::translate(tgCat, glm::vec3(-2.0f, 0.0f, 0.0f));
    tgCat = glm::scale(tgCat, glm::vec3(0.08f));
    tgCat = glm::rotate(tgCat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    tgCat = glm::rotate(tgCat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 catCenter = catRes->getCenter();
    tgCat = glm::translate(tgCat, -catCenter);
    catInst->modelTransform(tgCat);
    instances.push_back(catInst);

    ModelInstance* patInst = new ModelInstance(patricioRes);
    glm::mat4 tgPat(1.0f);
    tgPat = glm::translate(tgPat, glm::vec3(1.5f, 0.0f, 0.0f)); // Close to center
    tgPat = glm::rotate(tgPat, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 patCenter = patricioRes->getCenter();
    tgPat = glm::translate(tgPat, -patCenter);
    patInst->modelTransform(tgPat);
    instances.push_back(patInst);

    float radius = 5.0f;
    for (int i = 0; i < 20; ++i) {
        ModelInstance* treeInst = new ModelInstance(treeRes);
        float angle = (i / 20.0f) * 2.0f * glm::pi<float>();
        float x = cos(angle) * radius * (0.8f + (rand() % 40) / 100.0f);
        float z = sin(angle) * radius * (0.8f + (rand() % 40) / 100.0f);
        if (z > 4.5f) continue; 
        glm::mat4 tgTree(1.0f);
        tgTree = glm::translate(tgTree, glm::vec3(x, -0.5f, z));
        float randomScale = 0.03f + (rand() % 30) / 1000.0f;
        tgTree = glm::scale(tgTree, glm::vec3(randomScale));
        glm::vec3 treeCenter = treeRes->getCenter();
        tgTree = glm::translate(tgTree, -treeCenter);
        treeInst->modelTransform(tgTree);
        instances.push_back(treeInst);
    }

    camera.init(glm::vec3(-10.0f), glm::vec3(10.0f), false);
}

void Scene::update(float dt)
{
}

void Scene::render(const glm::vec3& lightPos, const glm::vec3& lightColor)
{
    for (ModelInstance* inst : instances) {
        inst->render(camera.getViewMatrix(), camera.getProjectMatrix(), lightPos, lightColor);
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
