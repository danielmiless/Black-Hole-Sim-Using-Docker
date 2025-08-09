/**
 * @file Camera.cpp
 * @brief Implementation of the enhanced orbital camera system
 */

#include "Camera.h"
#include "../utils/Logger.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camera::Camera(const Config& config)
    : m_target(0.0f, 0.0f, 0.0f)  // Always look at black hole center
    , m_radius(config.getFloat("camera.initialRadius", 6.34194e10f))
    , m_azimuth(0.0f)
    , m_elevation(M_PI / 2.0f)  // Start at equatorial plane
    , m_minRadius(config.getFloat("camera.minRadius", 1e10f))
    , m_maxRadius(config.getFloat("camera.maxRadius", 1e12f))
    , m_minElevation(0.01f)
    , m_maxElevation(M_PI - 0.01f)
    , m_orbitSpeed(config.getFloat("camera.orbitSpeed", 0.015f))
    , m_zoomSpeed(config.getFloat("camera.zoomSpeed", 25e9f))
    , m_dampingFactor(config.getFloat("camera.dampingFactor", 0.95f))
    , m_fov(config.getFloat("camera.fov", 60.0f))
    , m_aspectRatio(4.0f / 3.0f)  // Default aspect ratio
    , m_nearPlane(config.getFloat("camera.nearPlane", 1e8f))
    , m_farPlane(config.getFloat("camera.farPlane", 1e12f))
    , m_isDragging(false)
    , m_isMoving(false)
    , m_lastMouseX(0.0)
    , m_lastMouseY(0.0)
    , m_targetRadius(m_radius)
    , m_targetAzimuth(m_azimuth)
    , m_targetElevation(m_elevation) {
    
    Logger::getInstance().log(Logger::Level::INFO, 
        "Camera initialized at radius: " + std::to_string(m_radius));
}

void Camera::update(float deltaTime) {
    bool wasMoving = m_isMoving;
    
    // Apply smooth movement towards targets
    applySmoothing(deltaTime);
    
    // Check if we're still moving
    const float epsilon = 1e6f;  // Small threshold for "stopped" motion
    m_isMoving = (std::abs(m_radius - m_targetRadius) > epsilon) ||
                 (std::abs(m_azimuth - m_targetAzimuth) > 0.001f) ||
                 (std::abs(m_elevation - m_targetElevation) > 0.001f) ||
                 m_isDragging;
    
    // Log when movement state changes
    if (wasMoving != m_isMoving) {
        Logger::getInstance().log(Logger::Level::DEBUG, 
            m_isMoving ? "Camera started moving" : "Camera stopped moving");
    }
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 position = getPosition();
    glm::vec3 up(0.0f, 1.0f, 0.0f);  // World up vector
    
    return glm::lookAt(position, m_target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
}

glm::vec3 Camera::getPosition() const {
    return calculatePosition();
}

glm::vec3 Camera::getForward() const {
    glm::vec3 position = getPosition();
    return glm::normalize(m_target - position);
}

glm::vec3 Camera::getRight() const {
    glm::vec3 forward = getForward();
    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    return glm::normalize(glm::cross(forward, worldUp));
}

glm::vec3 Camera::getUp() const {
    glm::vec3 forward = getForward();
    glm::vec3 right = getRight();
    return glm::cross(right, forward);
}

void Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Camera aspect ratio updated: " + std::to_string(aspectRatio));
}

void Camera::processKeyboard(int key, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        const float moveSpeed = m_orbitSpeed * 2.0f;  // Faster for keyboard
        
        switch (key) {
            case GLFW_KEY_W: case GLFW_KEY_UP:
                m_targetElevation -= moveSpeed;
                break;
            case GLFW_KEY_S: case GLFW_KEY_DOWN:
                m_targetElevation += moveSpeed;
                break;
            case GLFW_KEY_A: case GLFW_KEY_LEFT:
                m_targetAzimuth -= moveSpeed;
                break;
            case GLFW_KEY_D: case GLFW_KEY_RIGHT:
                m_targetAzimuth += moveSpeed;
                break;
            case GLFW_KEY_Q:
                m_targetRadius *= 0.9f;  // Zoom in
                break;
            case GLFW_KEY_E:
                m_targetRadius *= 1.1f;  // Zoom out
                break;
            case GLFW_KEY_R:
                // Reset camera to default position
                m_targetRadius = 6.34194e10f;
                m_targetAzimuth = 0.0f;
                m_targetElevation = M_PI / 2.0f;
                Logger::getInstance().log(Logger::Level::INFO, "Camera reset to default position");
                break;
        }
        
        clampValues();
    }
}

void Camera::processMouseButton(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_isDragging = true;
        } else if (action == GLFW_RELEASE) {
            m_isDragging = false;
        }
    }
}

void Camera::processMouseMovement(double xpos, double ypos) {
    if (m_isDragging) {
        // Calculate mouse delta
        float deltaX = static_cast<float>(xpos - m_lastMouseX);
        float deltaY = static_cast<float>(ypos - m_lastMouseY);
        
        // Update target angles based on mouse movement
        m_targetAzimuth += deltaX * m_orbitSpeed;
        m_targetElevation -= deltaY * m_orbitSpeed;  // Invert Y for intuitive control
        
        clampValues();
    }
    
    // Always update last mouse position
    m_lastMouseX = xpos;
    m_lastMouseY = ypos;
}

void Camera::processMouseScroll(double xoffset, double yoffset) {
    // Zoom based on scroll direction
    float zoomFactor = 1.0f - static_cast<float>(yoffset) * 0.1f;
    m_targetRadius *= zoomFactor;
    
    clampValues();
    
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Camera zoom: " + std::to_string(m_targetRadius));
}

glm::vec3 Camera::calculatePosition() const {
    // Convert spherical coordinates to Cartesian
    float x = m_radius * std::sin(m_elevation) * std::cos(m_azimuth);
    float y = m_radius * std::cos(m_elevation);
    float z = m_radius * std::sin(m_elevation) * std::sin(m_azimuth);
    
    return glm::vec3(x, y, z) + m_target;
}

void Camera::applySmoothing(float deltaTime) {
    if (!glm::isnan(deltaTime) && deltaTime > 0.0f && deltaTime < 1.0f) {
        // Smooth interpolation towards target values
        float smoothingFactor = 1.0f - std::pow(m_dampingFactor, deltaTime * 60.0f);  // 60 FPS reference
        
        m_radius = glm::mix(m_radius, m_targetRadius, smoothingFactor);
        m_azimuth = glm::mix(m_azimuth, m_targetAzimuth, smoothingFactor);
        m_elevation = glm::mix(m_elevation, m_targetElevation, smoothingFactor);
    }
}

void Camera::clampValues() {
    // Clamp radius to valid range
    m_targetRadius = std::clamp(m_targetRadius, m_minRadius, m_maxRadius);
    
    // Clamp elevation to prevent camera flipping
    m_targetElevation = std::clamp(m_targetElevation, m_minElevation, m_maxElevation);
    
    // Normalize azimuth to [0, 2π] range
    while (m_targetAzimuth > 2.0f * M_PI) m_targetAzimuth -= 2.0f * M_PI;
    while (m_targetAzimuth < 0.0f) m_targetAzimuth += 2.0f * M_PI;
}