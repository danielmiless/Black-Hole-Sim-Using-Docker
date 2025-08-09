/**
 * @file Camera.h
 * @brief Enhanced camera system with orbital controls and smooth movement
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../utils/Config.h"
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

class Camera {
public:
    /**
     * @brief Camera constructor
     * @param config Configuration object
     */
    explicit Camera(const Config& config);
    
    /**
     * @brief Update camera state
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Get view matrix
     * @return 4x4 view matrix
     */
    glm::mat4 getViewMatrix() const;
    
    /**
     * @brief Get projection matrix
     * @return 4x4 projection matrix
     */
    glm::mat4 getProjectionMatrix() const;
    
    /**
     * @brief Get camera position in world space
     * @return Camera position vector
     */
    glm::vec3 getPosition() const;
    
    /**
     * @brief Get camera forward direction
     * @return Normalized forward vector
     */
    glm::vec3 getForward() const;
    
    /**
     * @brief Get camera right direction
     * @return Normalized right vector
     */
    glm::vec3 getRight() const;
    
    /**
     * @brief Get camera up direction
     * @return Normalized up vector
     */
    glm::vec3 getUp() const;
    
    /**
     * @brief Set aspect ratio for projection matrix
     * @param aspectRatio Width/height ratio
     */
    void setAspectRatio(float aspectRatio);
    
    /**
     * @brief Check if camera is currently moving (for optimization)
     * @return true if camera is moving
     */
    bool isMoving() const { return m_isMoving; }
    
    // Input processing methods
    void processKeyboard(int key, int action, int mods);
    void processMouseButton(int button, int action, int mods);
    void processMouseMovement(double xpos, double ypos);
    void processMouseScroll(double xoffset, double yoffset);

private:
    // Camera parameters
    glm::vec3 m_target;          ///< Point the camera looks at (black hole center)
    float m_radius;              ///< Distance from target
    float m_azimuth;             ///< Horizontal rotation angle
    float m_elevation;           ///< Vertical rotation angle
    
    // Movement constraints
    float m_minRadius;           ///< Minimum zoom distance
    float m_maxRadius;           ///< Maximum zoom distance
    float m_minElevation;        ///< Minimum vertical angle
    float m_maxElevation;        ///< Maximum vertical angle
    
    // Movement speeds
    float m_orbitSpeed;          ///< Mouse sensitivity for orbiting
    float m_zoomSpeed;           ///< Mouse wheel sensitivity
    float m_dampingFactor;       ///< Movement damping for smooth animation
    
    // Projection parameters
    float m_fov;                 ///< Field of view in degrees
    float m_aspectRatio;         ///< Width/height ratio
    float m_nearPlane;           ///< Near clipping plane
    float m_farPlane;            ///< Far clipping plane
    
    // Input state
    bool m_isDragging;           ///< Is left mouse button held?
    bool m_isMoving;             ///< Is camera currently moving?
    double m_lastMouseX;         ///< Last mouse X position
    double m_lastMouseY;         ///< Last mouse Y position
    
    // Smooth movement
    float m_targetRadius;        ///< Target radius for smooth zooming
    float m_targetAzimuth;       ///< Target azimuth for smooth rotation
    float m_targetElevation;     ///< Target elevation for smooth rotation
    
    /**
     * @brief Calculate camera position from spherical coordinates
     * @return Camera position in world space
     */
    glm::vec3 calculatePosition() const;
    
    /**
     * @brief Apply movement damping for smooth animation
     * @param deltaTime Time since last frame
     */
    void applySmoothing(float deltaTime);
    
    /**
     * @brief Clamp values to their valid ranges
     */
    void clampValues();
};