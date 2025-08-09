/**
 * @file Renderer.h
 * @brief OpenGL rendering system for the black hole simulation
 */

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "../physics/Physics.h"
#include "../objects/Object.h"
#include "../utils/Config.h"

class Renderer {
public:
    /**
     * @brief Construct renderer with given configuration
     * @param config Configuration object
     * @param width Window width
     * @param height Window height
     */
    Renderer(const Config& config, int width, int height);
    
    /**
     * @brief Destructor - cleanup OpenGL resources
     */
    ~Renderer();
    
    /**
     * @brief Render the current frame
     * @param camera Camera system for view/projection matrices
     * @param objects List of objects to render
     */
    void render(const Camera& camera, const std::vector<Object>& objects);
    
    /**
     * @brief Handle window resize
     * @param width New width
     * @param height New height
     */
    void resize(int width, int height);
    
    /**
     * @brief Toggle grid visualization
     */
    void toggleGrid() { m_showGrid = !m_showGrid; }
    
    /**
     * @brief Set adaptive quality mode
     * @param enabled Enable/disable adaptive quality
     */
    void setAdaptiveQuality(bool enabled) { m_adaptiveQuality = enabled; }

private:
    // Configuration
    Config m_config;
    int m_width, m_height;
    
    // Shader programs
    GLuint m_quadShaderProgram;     ///< Fullscreen quad shader
    GLuint m_gridShaderProgram;     ///< Spacetime grid shader
    GLuint m_computeShaderProgram;  ///< Ray tracing compute shader
    
    // OpenGL objects
    GLuint m_quadVAO;              ///< Fullscreen quad vertex array
    GLuint m_quadVBO;              ///< Fullscreen quad vertex buffer
    GLuint m_gridVAO;              ///< Grid vertex array
    GLuint m_gridVBO;              ///< Grid vertex buffer
    GLuint m_gridEBO;              ///< Grid element buffer
    
    // Textures
    GLuint m_rayTracingTexture;    ///< Output texture for ray tracing
    
    // Uniform buffer objects
    GLuint m_cameraUBO;            ///< Camera uniform buffer
    GLuint m_diskUBO;              ///< Accretion disk uniform buffer
    GLuint m_objectsUBO;           ///< Objects uniform buffer
    
    // Rendering state
    bool m_showGrid;               ///< Show spacetime grid
    bool m_adaptiveQuality;        ///< Enable adaptive quality
    int m_gridIndexCount;          ///< Number of grid indices
    
    // Compute resolution settings
    int m_staticWidth, m_staticHeight;     ///< High quality resolution
    int m_movingWidth, m_movingHeight;     ///< Lower quality for movement
    
    /**
     * @brief Initialize all OpenGL resources
     */
    void initializeGL();
    
    /**
     * @brief Create and compile shader programs
     */
    void createShaders();
    
    /**
     * @brief Load shader source from file
     * @param filename Path to shader file
     * @return Shader source code
     */
    std::string loadShaderSource(const std::string& filename);
    
    /**
     * @brief Compile a shader
     * @param source Shader source code
     * @param type Shader type (GL_VERTEX_SHADER, etc.)
     * @return Compiled shader ID
     */
    GLuint compileShader(const std::string& source, GLenum type);
    
    /**
     * @brief Create shader program from vertex and fragment shaders
     * @param vertexPath Path to vertex shader
     * @param fragmentPath Path to fragment shader
     * @return Linked shader program ID
     */
    GLuint createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    
    /**
     * @brief Create compute shader program
     * @param computePath Path to compute shader
     * @return Linked compute program ID
     */
    GLuint createComputeProgram(const std::string& computePath);
    
    /**
     * @brief Initialize fullscreen quad for final rendering
     */
    void initializeQuad();
    
    /**
     * @brief Generate and initialize spacetime grid
     * @param objects List of objects affecting spacetime curvature
     */
    void generateGrid(const std::vector<Object>& objects);
    
    /**
     * @brief Initialize uniform buffer objects
     */
    void initializeUBOs();
    
    /**
     * @brief Dispatch the compute shader for ray tracing
     * @param camera Current camera state
     * @param objects List of objects in the scene
     */
    void dispatchCompute(const Camera& camera, const std::vector<Object>& objects);
    
    /**
     * @brief Upload camera data to GPU
     * @param camera Current camera state
     */
    void uploadCameraUBO(const Camera& camera);
    
    /**
     * @brief Upload accretion disk data to GPU
     */
    void uploadDiskUBO();
    
    /**
     * @brief Upload object data to GPU
     * @param objects List of objects to upload
     */
    void uploadObjectsUBO(const std::vector<Object>& objects);
    
    /**
     * @brief Render the spacetime grid
     * @param viewProjMatrix Combined view-projection matrix
     */
    void renderGrid(const glm::mat4& viewProjMatrix);
    
    /**
     * @brief Render fullscreen quad with ray tracing result
     */
    void renderFullscreenQuad();
    
    /**
     * @brief Check for OpenGL errors and log them
     * @param operation Description of the operation being checked
     */
    void checkGLError(const std::string& operation);
};