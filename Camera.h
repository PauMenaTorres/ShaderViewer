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

    void init(glm::vec3 min, glm::vec3 max, bool adapt = true);


    void setType(CameraType type);

    void moveForward(float distance);
    void moveUp(float distance);
    void rotate(float angle);
    void restart();

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectMatrix();

    float getFOV();
    float setFOV(float newFov);

    void setAspectRatio(float ratio);

    void orbitX(float angle);
    void orbitY(float angle);
    void orbitZ(float angle);
private:

    float d, R;
    float FOV, zfar, znear, ra;
    float left, right, bottom, top;

    CameraType cameraType;

    glm::vec3 OBS, VRP, up_vector;
    glm::vec3 min_orig, max_orig;
    bool adapt_orig;

    glm::mat4 viewMatrix, projectMatrix;


    void updatePerspective();
    void updateOrthogonal();
    void updateLookAt();

};

#endif // CAMERA_H
