#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace eirikr {
    class Camera {
    private:
        float yaw;
        float pitch;
        float fov;
        float cameraSensitivity;
        float cameraSpeed;
        
        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        glm::vec3 worldUp;
        glm::vec3 cameraTarget;
        glm::vec3 cameraDirection; // readonly, from camera to target
        glm::vec3 cameraRight;
        glm::mat4 cameraView;
        glm::mat4 cameraProjection;
        
    public:
        enum mouseDirections {
            LEFT, RIGHT, UP, DOWN
        };
        
    public:
        Camera();
        
        inline void setCameraPos(glm::vec3 _pos);
        inline void setCameraFront(glm::vec3 _front);
        inline void setCameraUp(glm::vec3 _up);
        inline void setCameraTarget(glm::vec3 _target);
        
        inline void setYaw(float _y);
        inline void setPitch(float _p);
        inline void setFov(float _f);
        inline void setSensitivity(float _s);
        inline void setSpeed(float _multiplier, float _deltaTime);
        
        inline glm::vec3 getCameraPos();
        inline glm::vec3 getCameraFront();
        inline glm::vec3 getCameraUp();
        inline glm::vec3 getCameraTarget();
        inline glm::mat4 getCameraView();
        inline glm::mat4 getCameraProjection();
        
        inline float getYaw();
        inline float getPitch();
        inline float getFov();
        inline float getSensitivity();
        inline float getSpeed();
        
        glm::mat4 updateCameraView();
        glm::mat4 updateCameraProjection(float WIDTH, float HEIGHT, float nearPlane, float farPlane);
        glm::vec3 updateCameraFront(double currentX, double currentY, double lastX, double lastY);
        float updateFov(float xOffset, float yOffset);
        
        void processKeyboard(enum mouseDirections);
    };
    
    Camera::Camera() {
        yaw = -90.0f;
        pitch = 0.0f;
        fov = 45.0f;
        cameraSensitivity = 0.05f;
        cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        cameraDirection = glm::normalize(cameraPos - cameraTarget);
        worldUp = glm::vec3(0.0, 1.0f, 0.0f);
        cameraUp = glm::vec3(0.0, 1.0f, 0.0f);
        cameraRight = glm::normalize(glm::cross(worldUp, cameraDirection));
        cameraView = glm::lookAt(cameraPos, cameraPos + cameraFront, worldUp);
    }
    
    void Camera::setCameraPos(glm::vec3 _pos) { cameraPos = _pos; }
    void Camera::setCameraFront(glm::vec3 _front) { cameraFront = _front; }
    void Camera::setCameraUp(glm::vec3 _up) { cameraUp = _up; }
    void Camera::setCameraTarget(glm::vec3 _target) { cameraTarget = _target; }
    
    glm::vec3 Camera::getCameraPos() { return cameraPos; }
    glm::vec3 Camera::getCameraFront() { return cameraFront; }
    glm::vec3 Camera::getCameraUp() { return cameraUp; }
    glm::vec3 Camera::getCameraTarget() { return cameraTarget; }
    glm::mat4 Camera::getCameraView() { return cameraView; }
    glm::mat4 Camera::getCameraProjection() { return cameraProjection; }
    
    void Camera::setYaw(float _y) { yaw = _y; }
    void Camera::setPitch(float _p) { pitch = _p; }
    void Camera::setFov(float _f) { fov = _f; }
    void Camera::setSensitivity(float _s) { cameraSensitivity = _s; }
    void Camera::setSpeed(float _multiplier, float _deltaTime) { cameraSpeed = _multiplier * _deltaTime; }
    
    float Camera::getYaw() { return yaw; }
    float Camera::getPitch() { return pitch; }
    float Camera::getFov() { return fov; }
    float Camera::getSensitivity() { return cameraSensitivity; }
    float Camera::getSpeed() { return cameraSpeed; }
    
    glm::mat4 Camera::updateCameraView() {
        cameraView = glm::lookAt(cameraPos, cameraPos + cameraFront, worldUp);
        return cameraView;
    }
    
    glm::mat4 Camera::updateCameraProjection(float width, float height, float nearPlane, float farPlane) {
        cameraProjection = glm::perspective(glm::radians(fov), static_cast<float>(width / height), nearPlane, farPlane);
        return cameraProjection;
    }
    
    glm::vec3 Camera::updateCameraFront(double currentX, double currentY, double lastX, double lastY) {
        float xOffset = (currentX - lastX) * cameraSensitivity;
        float yOffset = (lastY - currentY) * cameraSensitivity;
        yaw += xOffset;
        pitch += yOffset;
        pitch = (pitch > 89.0f) ? 89.0f : pitch;
        pitch = (pitch < -89.0f) ? -89.0f : pitch;
        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront.y = sin(glm::radians(pitch));
        cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        return cameraFront;
    }
    
    float Camera::updateFov(float xOffset, float yOffset) {
        if (fov >= 1.0f && fov <= 90.0f) {
            fov -= yOffset;
        }
        else if (fov <= 1.0f) {
            fov = 1.0f;
        }
        else {
            fov = 90.0f;
        }
        return fov;
    }
    
    void Camera::processKeyboard(enum mouseDirections direction) {
        if (direction == mouseDirections::UP) {
            cameraPos += cameraSpeed * cameraFront;
        }
        else if (direction == mouseDirections::DOWN) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        else if (direction == mouseDirections::LEFT) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        else if (direction == mouseDirections::RIGHT) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
    }
    
}

#endif /* __CAMERA_HPP__ */
