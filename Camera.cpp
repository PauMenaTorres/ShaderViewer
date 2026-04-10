#include "Camera.h"

Camera::Camera()
{

}

void Camera::init()
{
    setType(CameraType::PERSPECTIVE);
    OBS = glm::vec3(0.0f);
    VRP = glm::vec3(0.0f, 0.0f, -1.0f);

}

void Camera::moveForward(float distance)
{
    OBS = OBS + distance * glm::normalize(VRP - OBS);
    VRP = VRP + distance * glm::normalize(VRP - OBS);
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
