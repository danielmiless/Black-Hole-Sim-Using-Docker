/**
 * @file Engine.h
 * @brief Main engine class header for the 3D Black Hole Simulation
 * 
 * This class manages the overall application state, including rendering,
 * physics simulation, input handling, and resource management.
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Renderer.h"
#include "../physics/Physics.h"
#include "../utils/Config.h"

class Engine {
public:
    /**
     * @brief Construct the engine with given configuration
     * @param config Configuration object containing simulation parameters
     */
    explicit Engine(const Config& config);
    
    /**
     * @brief Destructor - cleans up OpenGL resources and terminates GLFW
     */
    ~Engine();
    
    /**
     * @brief Check if the engine should continue running
     * @return true if the simulation should continue, false to exit
     */
    bool isRunning() const;
    
    /**
     * @brief Update simulation state
     * @param deltaTime Time since last frame in seconds
     */
    void update(float deltaTime);
    
    /**
     * @brief Render the current frame
     */
    void render();
    
private:
    GLFWwindow* m_window;                               ///< GLFW window handle
    std::unique_ptr<Camera> m_camera;                   ///< Camera system
    std::unique_ptr<Renderer> m_renderer;               ///< Rendering system
    std::unique_ptr<Physics> m_physics;                 ///< Physics simulation
    
    Config m_config;                                    ///< Configuration settings
    
    // Window properties
    int m_windowWidth;
    int m_windowHeight;
    const char* m_windowTitle;
    
    // Performance tracking
    double m_lastFrameTime;
    int m_frameCount;
    
    /**
     * @brief Initialize GLFW and create window
     * @return true on success, false on failure
     */
    bool initializeGLFW();
    
    /**
     * @brief Initialize OpenGL context and GLEW
     * @return true on success, false on failure
     */
    bool initializeOpenGL();
    
    /**
     * @brief Set up input callbacks
     */
    void setupCallbacks();
    
    /**
     * @brief Static callback functions for GLFW
     */
    static void errorCallback(int error, const char* description);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};
