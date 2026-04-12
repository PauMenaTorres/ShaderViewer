#include "Camera.h"

Camera::Camera()
{

}
void Camera::init()
{
    setType(CameraType::PERSPECTIVE);

    OBS = glm::vec3(0.0f, 0.0f, 2.0f);
    VRP = glm::vec3(0.0f, 0.0f, 0.0f);

    up_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    FOV = glm::radians(60.0f);
    znear = 0.1f;
    zfar = 100.0f;
    ra = 1.0f;

    updateLookAt();
    updatePerspective();
}

void Camera::moveForward(float distance)
{
    OBS = OBS + distance * glm::normalize(VRP - OBS);
    VRP = VRP + distance * glm::normalize(VRP - OBS);

    updateLookAt();
}

void Camera::rotate(float angle)
{
    glm::mat4 TG(1.0f);

    TG = glm::translate(TG, OBS);
    TG = glm::rotate(TG, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    TG = glm::translate(TG, -OBS);

    glm::vec4 newVRP = TG * glm::vec4(VRP, 1.0f);
    VRP = glm::vec3(newVRP);

    updateLookAt();
}

void Camera::updatePerspective()
{
    projectMatrix = glm::perspective(FOV, ra, znear, zfar);
}

void Camera::updateOrthogonal()
{
    projectMatrix = glm::ortho(left, right, bottom, top, znear, zfar);
}

void Camera::updateLookAt()
{
    viewMatrix = glm::lookAt(OBS, VRP, up_vector);
}

glm::mat4 Camera::getViewMatrix()
{
    return viewMatrix;
}

glm::mat4 Camera::getProjectMatrix()
{
    return projectMatrix;
}

void Camera::setType(CameraType type)
{
    cameraType = type;
}

