#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraType
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera
{

public:

    Camera();

    void init();

    void setType(CameraType type);

    void moveForward(float distance);
    void rotate(float angle);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectMatrix();

    void setAspectRatio(float ratio);

private:

    float FOV, zfar, znear, ra;
    float left, right, bottom, top;

    CameraType cameraType;

    glm::vec3 OBS, VRP, up_vector;

    glm::mat4 viewMatrix, projectMatrix;


    void updatePerspective();
    void updateOrthogonal();
    void updateLookAt();

};

#endif // CAMERA_H
