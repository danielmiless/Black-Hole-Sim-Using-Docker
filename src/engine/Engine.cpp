/**
 * @file Engine.cpp
 * @brief Implementation of the main engine class
 */

#include "Engine.h"
#include "../utils/Logger.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

Engine::Engine(const Config& config) 
    : m_window(nullptr)
    , m_config(config)
    , m_windowWidth(config.getInt("window.width", 1200))
    , m_windowHeight(config.getInt("window.height", 800))
    , m_windowTitle("Enhanced 3D Black Hole Simulation v2.0")
    , m_lastFrameTime(0.0)
    , m_frameCount(0) {
    
    if (!initializeGLFW()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    if (!initializeOpenGL()) {
        throw std::runtime_error("Failed to initialize OpenGL");
    }
    
    // Initialize subsystems
    m_camera = std::make_unique<Camera>(m_config);
    m_renderer = std::make_unique<Renderer>(m_config, m_windowWidth, m_windowHeight);
    m_physics = std::make_unique<Physics>(m_config);
    
    setupCallbacks();
    
    Logger::getInstance().log(Logger::Level::INFO, 
        "Engine initialized: " + std::to_string(m_windowWidth) + "x" + std::to_string(m_windowHeight));
}

Engine::~Engine() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
    Logger::getInstance().log(Logger::Level::INFO, "Engine destroyed");
}

bool Engine::isRunning() const {
    return m_window && !glfwWindowShouldClose(m_window);
}

void Engine::update(float deltaTime) {
    glfwPollEvents();
    
    m_camera->update(deltaTime);
    m_physics->update(deltaTime);
    
    // Update window title with performance info occasionally
    m_frameCount++;
    if (m_frameCount % 60 == 0) {  // Every 60 frames
        double fps = 1.0 / deltaTime;
        std::string title = std::string(m_windowTitle) + " - " + 
                           std::to_string(static_cast<int>(fps)) + " FPS";
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

void Engine::render() {
    m_renderer->render(*m_camera, m_physics->getObjects());
    glfwSwapBuffers(m_window);
}

bool Engine::initializeGLFW() {
    glfwSetErrorCallback(errorCallback);
    
    if (!glfwInit()) {
        Logger::getInstance().log(Logger::Level::ERROR, "Failed to initialize GLFW");
        return false;
    }
    
    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required for macOS
    
    // Additional hints for better compatibility
    glfwWindowHint(GLFW_SAMPLES, 4);  // 4x MSAA
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle, nullptr, nullptr);
    if (!m_window) {
        Logger::getInstance().log(Logger::Level::ERROR, "Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);
    
    // Enable VSync
    glfwSwapInterval(1);
    
    return true;
}

bool Engine::initializeOpenGL() {
    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Failed to initialize GLEW: " + std::string((const char*)glewGetErrorString(glewError)));
        return false;
    }
    
    // Log OpenGL information
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    
    Logger::getInstance().log(Logger::Level::INFO, "OpenGL Renderer: " + std::string((const char*)renderer));
    Logger::getInstance().log(Logger::Level::INFO, "OpenGL Version: " + std::string((const char*)version));
    Logger::getInstance().log(Logger::Level::INFO, "GLSL Version: " + std::string((const char*)glslVersion));
    
    // Set OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);  // Dark blue background
    
    return true;
}

void Engine::setupCallbacks() {
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
}

// Static callback implementations
void Engine::errorCallback(int error, const char* description) {
    Logger::getInstance().log(Logger::Level::ERROR, 
        "GLFW Error " + std::to_string(error) + ": " + std::string(description));
}

void Engine::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    if (engine->m_camera) {
        engine->m_camera->processKeyboard(key, action, mods);
    }
    
    if (engine->m_physics) {
        engine->m_physics->processKeyboard(key, action, mods);
    }
}

void Engine::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    
    if (engine->m_camera) {
        engine->m_camera->processMouseButton(button, action, mods);
    }
}

void Engine::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    
    if (engine->m_camera) {
        engine->m_camera->processMouseMovement(xpos, ypos);
    }
}

void Engine::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    
    if (engine->m_camera) {
        engine->m_camera->processMouseScroll(xoffset, yoffset);
    }
}

void Engine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    
    glViewport(0, 0, width, height);
    engine->m_windowWidth = width;
    engine->m_windowHeight = height;
    
    if (engine->m_renderer) {
        engine->m_renderer->resize(width, height);
    }
    
    if (engine->m_camera) {
        engine->m_camera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }
}
