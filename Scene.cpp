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
    
    if (waterInst) {
        delete waterInst;
        waterInst = nullptr;
    }
    
    for (auto const& pair : resources) {
        delete pair.second;
    }
    resources.clear();
}

ModelResource* Scene::getResource(const QString& path, const QString& fragShader)
{
    return getResource(path, ":/vertexModel.vert", fragShader);
}

ModelResource* Scene::getResource(const QString& path, const QString& vertShader, const QString& fragShader)
{
    QString key = path + vertShader + fragShader;
    if (resources.find(key) == resources.end()) {
        ModelResource* res = new ModelResource();
        res->init(path, vertShader, fragShader);
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
    
    if (waterInst) {
        delete waterInst;
        waterInst = nullptr;
    }
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

void Scene::loadWaterScene()
{
    init();

    ModelResource* patricioRes = getResource("Models3D/Patricio.obj", ":/geometryPass.frag");

    // --- Patricio: Colocado justo encima del agua (pies rozando Y = 0.0f) ---
    ModelInstance* patricioInst = new ModelInstance(patricioRes);
    patricioInst->modelTransform(createModelTransform(patricioRes, glm::vec3(0.0f, 1.8f, 0.0f), 1.6f));
    instances.push_back(patricioInst);

    // --- Geometría Base del Agua (Paso 1 y 2) ---
    ModelResource* waterRes = getResource("Models3D/water_quad.obj", ":/water.vert", ":/water.frag");
    waterInst = new ModelInstance(waterRes);
    glm::mat4 waterTg(1.0f);
    waterTg = glm::translate(waterTg, glm::vec3(0.0f, 0.0f, 0.0f)); 
    waterTg = glm::scale(waterTg, glm::vec3(5.0f, 1.0f, 5.0f));      // Hacemos el quad grande
    waterInst->modelTransform(waterTg);

    // --- Cámara: Ajustamos para ver el agua desde arriba/diagonal ---
    glm::vec3 sceneMin = glm::vec3(-5.0f, -0.5f, -5.0f);
    glm::vec3 sceneMax = glm::vec3(5.0f, 2.0f, 5.0f);
    camera.init(sceneMin, sceneMax, true);
    camera.orbitY(glm::radians(45.0f));  // Rotación diagonal
    camera.orbitX(glm::radians(-20.0f)); // Miramos un poco hacia abajo
}

void Scene::update()
{
}

void Scene::render(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue)
{
    renderModelsOnly(lightPos, lightColor, attValue);
    renderWaterOnly(lightPos, lightColor, attValue);
}

void Scene::renderModelsOnly(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, const glm::vec4& clipPlane)
{
    for (ModelInstance* inst : instances) {
        inst->render(camera.getViewMatrix(), camera.getProjectMatrix(), lightPos, lightColor, attValue, clipPlane);
    }
}

void Scene::renderWaterOnly(const glm::vec3& lightPos, const glm::vec3& lightColor, float attValue, GLuint reflectionTex, GLuint refractionTex, GLuint dudvTex, GLuint normalTex, float moveFactor, float waveStrength, float waterShininess)
{
    if (waterInst) {
        waterInst->renderWater(camera.getViewMatrix(), camera.getProjectMatrix(), lightPos, lightColor, attValue, reflectionTex, refractionTex, dudvTex, normalTex, moveFactor, camera.getOBS(), waveStrength, waterShininess);
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
