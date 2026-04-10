#include "Scene.h"

Scene::Scene()
{

}

void Scene::init()
{
    ModelOBJ* homer = new ModelOBJ;
    homer->init("Models3D/HomerProves.obj", ":/vertexModel.vert", ":/fragmentRed.frag");
    glm::mat4 tgHomer(1.0f);
    tgHomer = glm::translate(tgHomer, glm::vec3(0.3f, 0.0f, 0.0f));
    tgHomer = glm::scale(tgHomer, glm::vec3(0.5f));
    homer->modelTransform(tgHomer);

    models.push_back(homer);

    ModelOBJ* tree = new ModelOBJ;
    tree->init("Models3D/tree.obj", ":/vertexModel.vert", ":/fragmentGreen.frag");
    glm::mat4 tgTree(1.0f);
    tgTree = glm::translate(tgTree, glm::vec3(-0.4f, 0.0f, 0.0f));
    tgTree = glm::scale(tgTree, glm::vec3(0.05f));
    tree->modelTransform(tgTree);

    models.push_back(tree);

    camera.init();
}

void Scene::render()
{

}

void Scene::moveForward(float distance)
{
    camera.moveForward(distance);
}
