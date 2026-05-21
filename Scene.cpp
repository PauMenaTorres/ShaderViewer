#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>


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
    float scaleFactorX = 1.0f;
    float scaleFactorY = 1.0f;
    float scaleFactorZ = 1.0f;

    if (currentHeight > 0.0001f) {
        float scaleFactor = targetHeight / currentHeight;
        scaleFactorX = scaleFactor;
        scaleFactorY = scaleFactor;
        scaleFactorZ = scaleFactor;
    } else {
        // Safe fall-back for flat/2D planes (e.g. water_quad)
        scaleFactorX = targetHeight;
        scaleFactorY = 1.0f;
        scaleFactorZ = targetHeight;
    }

    glm::mat4 tg(1.0f);
    tg = glm::translate(tg, position);                               // 3. colocar en posición final
    tg = glm::scale(tg, glm::vec3(scaleFactorX, scaleFactorY, scaleFactorZ)); // 2. escalar a la altura deseada
    tg = glm::translate(tg, -center);                                // 1. centrar el objeto en el origen
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

    // 1. Multiple mountains surrounding the water to create a valley lake
    ModelResource* mountainRes = getResource("Models3D/mountain.obj", ":/geometryPass.frag");

    // Positions, heights, and rotation angles (in degrees) for a beautiful, organic ring of 8 mountains
    struct MountainConfig {
        glm::vec3 pos;
        float height;
        float rotY;
    } mountains[] = {
        { glm::vec3(0.0f, 0.0f, -5.2f),   3.5f, 0.0f   },
        { glm::vec3(3.8f, -0.08f, -3.8f), 4.0f, 45.0f  },
        { glm::vec3(5.2f, -0.05f, 0.0f),  3.7f, 90.0f  },
        { glm::vec3(3.8f, -0.05f, 3.8f),  3.4f, 135.0f },
        { glm::vec3(0.0f, -0.1f, 5.2f),   3.2f, 180.0f },
        { glm::vec3(-3.8f, -0.1f, 3.8f),  3.8f, 225.0f },
        { glm::vec3(-5.2f, 0.0f, 0.0f),   3.3f, 270.0f },
        { glm::vec3(-3.8f, 0.0f, -3.8f),  3.6f, 315.0f }
    };

    for (int i = 0; i < 8; ++i) {
        ModelInstance* inst = new ModelInstance(mountainRes);
        glm::mat4 t = createModelTransform(mountainRes, mountains[i].pos, mountains[i].height);
        t = glm::rotate(t, glm::radians(mountains[i].rotY), glm::vec3(0.0f, 1.0f, 0.0f));
        inst->modelTransform(t);
        instances.push_back(inst);
    }

    // 2. Sailboat floating gracefully in the lake center (raised to float at water level)
    ModelResource* boatRes = getResource("Models3D/boat.obj", ":/geometryPass.frag");
    ModelInstance* boatInst = new ModelInstance(boatRes);
    glm::mat4 boatT = createModelTransform(boatRes, glm::vec3(-0.4f, 0.33f, 0.4f), 0.8f);
    boatT = glm::rotate(boatT, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    boatInst->modelTransform(boatT);
    instances.push_back(boatInst);

    // 3. Dolphin leaping elegantly near the sailboat
    ModelResource* dolphinRes = getResource("Models3D/dolphin.obj", ":/geometryPass.frag");
    ModelInstance* dolphinInst = new ModelInstance(dolphinRes);
    glm::mat4 dolphinT = createModelTransform(dolphinRes, glm::vec3(0.4f, 0.38f, -0.4f), 0.5f);
    dolphinT = glm::rotate(dolphinT, glm::radians(-135.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    dolphinT = glm::rotate(dolphinT, glm::radians(-75.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch right-side up horizontally
    dolphinInst->modelTransform(dolphinT);
    instances.push_back(dolphinInst);

    // 4. Spaceship hovering further back and aligned to reflect beautifully in the lake
    ModelResource* shipRes = getResource("Models3D/ship.obj", ":/geometryPass.frag");
    ModelInstance* shipInst = new ModelInstance(shipRes);
    glm::mat4 shipT = createModelTransform(shipRes, glm::vec3(0.0f, 2.5f, -2.0f), 0.4f); // Placed further back, smaller to fit in the sky beautifully
    shipT = glm::rotate(shipT, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Pointing towards the camera view
    shipInst->modelTransform(shipT);
    instances.push_back(shipInst);

    // 5. A larger sailboat placed further back in the opposite quadrant to balance composition and show huge reflections (raised to float at water level)
    ModelInstance* bigBoatInst = new ModelInstance(boatRes);
    glm::mat4 bigBoatT = createModelTransform(boatRes, glm::vec3(1.2f, 0.58f, -1.2f), 1.5f); // Twice the size of the first boat
    bigBoatT = glm::rotate(bigBoatT, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    bigBoatInst->modelTransform(bigBoatT);
    instances.push_back(bigBoatInst);

    // --- Base Water Geometry (using standard createModelTransform robustly) ---
    ModelResource* waterRes = getResource("Models3D/water_quad.obj", ":/water.vert", ":/water.frag");
    waterInst = new ModelInstance(waterRes);
    waterInst->modelTransform(createModelTransform(waterRes, glm::vec3(0.0f, 0.0f, 0.0f), 6.0f));

    // --- Camera: Setup to place the observer INSIDE the lake valley ---
    glm::vec3 sceneMin = glm::vec3(-6.0f, -0.5f, -6.0f);
    glm::vec3 sceneMax = glm::vec3(6.0f, 4.0f, 6.0f);
    camera.init(sceneMin, sceneMax, true);
    
    // Position camera dynamically at a low angle, very close to the boats and at their height (buoyancy level)
    camera.setOBS(glm::vec3(-0.6f, 0.42f, 1.2f));
    camera.setVRP(glm::vec3(0.5f, 0.45f, -0.8f));
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
