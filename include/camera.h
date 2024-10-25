#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <bits/stdc++.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
    public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 look;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float rotateSpeed = 2.5f;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    void updateCenter(glm::vec3 center) {
        look = center;
    }
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, look, Up);
    }
    void revolve(float deltaTime, glm::vec3 center) {
        // Look at the center of the prism
        updateCenter(center);
        // Rotate position vector by translating look vector to zero.
        glm::vec3 v = Position - look;
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, rotateSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec4 temp = trans * glm::vec4(v, 1.0f);
        Position = glm::vec3(temp) + look;
    }
    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime, glm::vec3 center)
    {
        // Look at the center of the prism
        updateCenter(center);
        float velocity = MovementSpeed * deltaTime;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
    }

    void setPosition(glm::vec3 position, glm::vec3 center) {
        updateCenter(center);
        Position = position;
    }
    glm::vec3 cameraRelative(Camera_Movement direction, float deltaTime)
    {
        // Finding vectors relative to the camera
        glm::vec3 f = glm::normalize(look - Position);
        glm::vec3 r = glm::normalize(glm::cross(f, WorldUp));
        glm::vec3 u = glm::normalize(glm::cross(r, f));

        float velocity = MovementSpeed * deltaTime;
        if (direction == UP)
            return u * velocity;
        if (direction == DOWN)
            return -u * velocity;
        if (direction == RIGHT)
            return r * velocity;
        if (direction == LEFT)
            return -r * velocity;
        if (direction == FORWARD)
            return f * velocity;
        if (direction == BACKWARD)
            return -f * velocity;

        return glm::vec3(0.0f, 0.0f, 0.0f);
    }
    private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
