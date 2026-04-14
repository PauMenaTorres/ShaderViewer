#include "Scene.h"

Scene::Scene()
{

}

void Scene::init()
{
    ModelOBJ* patricio = new ModelOBJ;
    patricio->init("Models3D/Patricio.obj", ":/vertexModel.vert", ":/fragmentModel.frag");
    glm::mat4 tgPatricio(1.0f);

    glm::vec3 center = patricio->getCenter();

    tgPatricio = glm::translate(tgPatricio, glm::vec3(0.3f, 0.0f, 0.0f));
    tgPatricio = glm::scale(tgPatricio, glm::vec3(0.2f));
    tgPatricio = glm::translate(tgPatricio, -center);

    patricio->modelTransform(tgPatricio);

    models.push_back(patricio);

    tgPatricio = glm::translate(tgPatricio, -center);
    ModelOBJ* tree = new ModelOBJ;
    tree->init("Models3D/tree.obj", ":/vertexModel.vert", ":/fragmentModel.frag");
    glm::mat4 tgTree(1.0f);
    tgTree = glm::translate(tgTree, glm::vec3(-0.8f, 0.0f, 0.0f));
    tgTree = glm::scale(tgTree, glm::vec3(0.05f));
    tgTree = glm::translate(tgTree, -center);

    tree->modelTransform(tgTree);

    models.push_back(tree);

    camera.init();
}

void Scene::render()
{
    glm::mat4 viewMat = camera.getViewMatrix();
    glm::mat4 projMat = camera.getProjectMatrix();

    for(ModelOBJ* model : models)
    {
        model->render(viewMat, projMat);
    }

}

Camera& Scene::getCamera()
{
    return camera;
}
