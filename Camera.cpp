#include "Camera.h"

Camera::Camera()
{

}
void Camera::init(glm::vec3 min, glm::vec3 max, bool adapt)
{
    min_orig = min;
    max_orig = max;
    adapt_orig = adapt;
    setType(CameraType::PERSPECTIVE);


    if (adapt) {
        R = glm::distance(max_orig, min_orig) / 2.0f;
        d = R + 2.0f;
        VRP = (min_orig + max_orig) / 2.0f;
        OBS = VRP + glm::vec3(0.0f, 0.0f, d);
        FOV = 2 * glm::asin(R / d);
        znear = d - R;
        zfar = d + R;
    } else {
        VRP = glm::vec3(0.0f, 0.0f, 0.0f); 
        OBS = glm::vec3(0.0f, 0.0f, 6.0f);
        FOV = glm::radians(90.0f);
        znear = 0.1f;
        zfar = 1000.0f;
    }

    up_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    updateLookAt();
    updatePerspective();
}


void Camera::moveForward(float distance)
{
    OBS = OBS + distance * glm::normalize(VRP - OBS);
    VRP = VRP + distance * glm::normalize(VRP - OBS);

    updateLookAt();
}

void Camera::moveUp(float distance)
{
    glm::vec3 up = glm::normalize(up_vector);

    OBS += distance * up;
    VRP += distance * up;

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

void Camera::orbitX(float angle)
{
    glm::vec3 right = glm::normalize(glm::cross(VRP - OBS, up_vector));
    
    glm::mat4 TG(1.0f);
    TG = glm::translate(TG, VRP);
    TG = glm::rotate(TG, angle, right);
    TG = glm::translate(TG, -VRP);

    glm::vec4 newOBS = TG * glm::vec4(OBS, 1.0f);
    OBS = glm::vec3(newOBS);

    glm::vec4 newUp = TG * glm::vec4(up_vector, 0.0f);
    up_vector = glm::normalize(glm::vec3(newUp));

    glm::vec3 forward = glm::normalize(VRP - OBS);
    glm::vec3 rightVec = glm::normalize(glm::cross(forward, up_vector));
    up_vector = glm::normalize(glm::cross(rightVec, forward));

    updateLookAt();
}

void Camera::orbitY(float angle)
{
    glm::vec3 up = glm::normalize(up_vector);
    
    glm::mat4 TG(1.0f);
    TG = glm::translate(TG, VRP);
    TG = glm::rotate(TG, angle, up);
    TG = glm::translate(TG, -VRP);

    glm::vec4 newOBS = TG * glm::vec4(OBS, 1.0f);
    OBS = glm::vec3(newOBS);

    glm::vec4 newUp = TG * glm::vec4(up_vector, 0.0f);
    up_vector = glm::normalize(glm::vec3(newUp));

    glm::vec3 forward = glm::normalize(VRP - OBS);
    glm::vec3 rightVec = glm::normalize(glm::cross(forward, up_vector));
    up_vector = glm::normalize(glm::cross(rightVec, forward));

    updateLookAt();
}

void Camera::orbitZ(float angle)
{
    glm::vec3 forward = glm::normalize(VRP - OBS);
    
    glm::mat4 TG(1.0f);
    glm::vec4 newUp = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(up_vector, 0.0f);
    up_vector = glm::normalize(glm::vec3(newUp));

    glm::vec3 rightVec = glm::normalize(glm::cross(forward, up_vector));
    up_vector = glm::normalize(glm::cross(rightVec, forward));

    updateLookAt();
}

void Camera::restart()
{
    init(min_orig, max_orig, adapt_orig);
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

float Camera::getFOV()
{
    return FOV;
}

float Camera::setFOV(float newFov)
{
    FOV = newFov;
    return FOV;
}

void Camera::setType(CameraType type)
{
    cameraType = type;
}

void Camera::setAspectRatio(float ratio)
{
    ra = ratio;
    updatePerspective();
}
