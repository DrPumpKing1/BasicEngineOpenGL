#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement {
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FORWARD,
    BACKWARD,
};


const float DEFAULT_YAW         = -90.0f;
const float DEFAULT_PITCH       =  0.0f;
const float DEFAULT_SPEED       =  2.5f;
const float DEFAULT_ALT_SPEED   =  5.0f;
const float DEFAULT_SENSITIVITY =  0.1f;
const float DEFAULT_ZOOM        =  45.0f;
const glm::vec3 DEFAULT_FRONT   = glm::vec3(0.0f, 0.0f, -1.0f);

const float MIN_PITCH = -89.0f;
const float MAX_PITCH = 89.0f;
const float MIN_ZOOM = 1.0f;
const float MAX_ZOOM = 45.0f;

class Camera
{
private:
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float altMovementSpeed;
    float mouseSensitivity;
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH);

    glm::mat4 GetViewMatrix() const;

    void ProcessKeyboardSpeed(bool alt);
    void ProcessKeyboardMovement(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    void NormalizeCameraVectors();
};