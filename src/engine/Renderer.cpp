/**
 * @file Renderer.cpp
 * @brief Implementation of the OpenGL rendering system
 */

#include "Renderer.h"
#include "../utils/Logger.h"
#include "../physics/BlackHole.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

Renderer::Renderer(const Config& config, int width, int height)
    : m_config(config)
    , m_width(width)
    , m_height(height)
    , m_quadShaderProgram(0)
    , m_gridShaderProgram(0)
    , m_computeShaderProgram(0)
    , m_quadVAO(0)
    , m_quadVBO(0)
    , m_gridVAO(0)
    , m_gridVBO(0)
    , m_gridEBO(0)
    , m_rayTracingTexture(0)
    , m_cameraUBO(0)
    , m_diskUBO(0)
    , m_objectsUBO(0)
    , m_showGrid(config.getBool("rendering.enableGrid", true))
    , m_adaptiveQuality(config.getBool("rendering.adaptiveQuality", true))
    , m_gridIndexCount(0)
    , m_staticWidth(config.getInt("rendering.staticResolution[0]", 800))
    , m_staticHeight(config.getInt("rendering.staticResolution[1]", 600))
    , m_movingWidth(config.getInt("rendering.movingResolution[0]", 400))
    , m_movingHeight(config.getInt("rendering.movingResolution[1]", 300)) {
    
    initializeGL();
    Logger::getInstance().log(Logger::Level::INFO, "Renderer initialized");
}

Renderer::~Renderer() {
    // Clean up OpenGL resources
    if (m_quadShaderProgram) glDeleteProgram(m_quadShaderProgram);
    if (m_gridShaderProgram) glDeleteProgram(m_gridShaderProgram);
    if (m_computeShaderProgram) glDeleteProgram(m_computeShaderProgram);
    
    if (m_quadVAO) glDeleteVertexArrays(1, &m_quadVAO);
    if (m_quadVBO) glDeleteBuffers(1, &m_quadVBO);
    if (m_gridVAO) glDeleteVertexArrays(1, &m_gridVAO);
    if (m_gridVBO) glDeleteBuffers(1, &m_gridVBO);
    if (m_gridEBO) glDeleteBuffers(1, &m_gridEBO);
    
    if (m_rayTracingTexture) glDeleteTextures(1, &m_rayTracingTexture);
    
    if (m_cameraUBO) glDeleteBuffers(1, &m_cameraUBO);
    if (m_diskUBO) glDeleteBuffers(1, &m_diskUBO);
    if (m_objectsUBO) glDeleteBuffers(1, &m_objectsUBO);
    
    Logger::getInstance().log(Logger::Level::INFO, "Renderer destroyed");
}

void Renderer::render(const Camera& camera, const std::vector<Object>& objects) {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Dispatch compute shader for ray tracing
    dispatchCompute(camera, objects);
    
    // Render fullscreen quad with ray tracing result
    renderFullscreenQuad();
    
    // Render spacetime grid if enabled
    if (m_showGrid) {
        glm::mat4 viewMatrix = camera.getViewMatrix();
        glm::mat4 projMatrix = camera.getProjectionMatrix();
        glm::mat4 viewProjMatrix = projMatrix * viewMatrix;
        
        renderGrid(viewProjMatrix);
    }
    
    checkGLError("render frame");
}

void Renderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
    
    Logger::getInstance().log(Logger::Level::INFO, 
        "Renderer resized: " + std::to_string(width) + "x" + std::to_string(height));
}

void Renderer::initializeGL() {
    createShaders();
    initializeQuad();
    initializeUBOs();
    
    // Create ray tracing texture
    glGenTextures(1, &m_rayTracingTexture);
    glBindTexture(GL_TEXTURE_2D, m_rayTracingTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Initialize with default resolution
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_staticWidth, m_staticHeight, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    checkGLError("initialize GL");
}

void Renderer::createShaders() {
    try {
        // Create shader programs
        m_quadShaderProgram = createShaderProgram("shaders/vertex.vert", "shaders/fragment.frag");
        m_gridShaderProgram = createShaderProgram("shaders/grid.vert", "shaders/grid.frag");
        m_computeShaderProgram = createComputeProgram("shaders/geodesic.comp");
        
        Logger::getInstance().log(Logger::Level::INFO, "All shaders compiled successfully");
        
    } catch (const std::exception& e) {
        Logger::getInstance().log(Logger::Level::ERROR, 
            "Shader compilation failed: " + std::string(e.what()));
        throw;
    }
}

std::string Renderer::loadShaderSource(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open shader file: " + filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Renderer::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        
        glDeleteShader(shader);
        throw std::runtime_error("Shader compilation failed: " + std::string(log.data()));
    }
    
    return shader;
}

GLuint Renderer::createShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexSource = loadShaderSource(vertexPath);
    std::string fragmentSource = loadShaderSource(fragmentPath);
    
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        throw std::runtime_error("Shader program linking failed: " + std::string(log.data()));
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

GLuint Renderer::createComputeProgram(const std::string& computePath) {
    std::string computeSource = loadShaderSource(computePath);
    GLuint computeShader = compileShader(computeSource, GL_COMPUTE_SHADER);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        
        glDeleteShader(computeShader);
        glDeleteProgram(program);
        throw std::runtime_error("Compute program linking failed: " + std::string(log.data()));
    }
    
    glDeleteShader(computeShader);
    return program;
}

void Renderer::initializeQuad() {
    // Fullscreen quad vertices (NDC coordinates)
    float quadVertices[] = {
        // Positions   // Texture Coords
        -1.0f,  1.0f,  0.0f, 1.0f,  // Top Left
        -1.0f, -1.0f,  0.0f, 0.0f,  // Bottom Left
         1.0f, -1.0f,  1.0f, 0.0f,  // Bottom Right
         1.0f,  1.0f,  1.0f, 1.0f   // Top Right
    };
    
    unsigned int quadIndices[] = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };
    
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    
    GLuint EBO;
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(m_quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Renderer::generateGrid(const std::vector<Object>& objects) {
    const int gridSize = m_config.getInt("rendering.gridSize", 25);
    const float spacing = m_config.getFloat("rendering.gridSpacing", 1e10f);
    
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    
    // Generate grid vertices with spacetime curvature
    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            float worldX = (x - gridSize / 2) * spacing;
            float worldZ = (z - gridSize / 2) * spacing;
            float worldY = 0.0f;
            
            // Apply spacetime curvature from all massive objects
            for (const auto& obj : objects) {
                glm::vec3 objPos = obj.getPosition();
                double mass = obj.getMass();
                
                if (mass > 0) {
                    // Calculate Schwarzschild radius
                    const double G = 6.67430e-11;
                    const double c = 299792458.0;
                    double rs = 2.0 * G * mass / (c * c);
                    
                    double dx = worldX - objPos.x;
                    double dz = worldZ - objPos.z;
                    double dist = std::sqrt(dx * dx + dz * dz);
                    
                    if (dist > rs) {
                        // Gravitational potential creates spacetime curvature
                        double curvature = 2.0 * std::sqrt(rs * (dist - rs));
                        worldY += static_cast<float>(curvature) - 3e10f;
                    } else {
                        // Inside event horizon - extreme curvature
                        worldY += 2.0f * static_cast<float>(std::sqrt(rs * rs)) - 3e10f;
                    }
                }
            }
            
            vertices.emplace_back(worldX, worldY, worldZ);
        }
    }
    
    // Generate grid line indices
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            int i = z * (gridSize + 1) + x;
            
            // Horizontal lines
            if (x < gridSize) {
                indices.push_back(i);
                indices.push_back(i + 1);
            }
            
            // Vertical lines
            if (z < gridSize) {
                indices.push_back(i);
                indices.push_back(i + gridSize + 1);
            }
        }
    }
    
    // Upload to GPU
    if (m_gridVAO == 0) glGenVertexArrays(1, &m_gridVAO);
    if (m_gridVBO == 0) glGenBuffers(1, &m_gridVBO);
    if (m_gridEBO == 0) glGenBuffers(1, &m_gridEBO);
    
    glBindVertexArray(m_gridVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gridEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    m_gridIndexCount = indices.size();
    
    glBindVertexArray(0);
}

void Renderer::initializeUBOs() {
    // Camera UBO
    glGenBuffers(1, &m_cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 128, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_cameraUBO);
    
    // Disk UBO
    glGenBuffers(1, &m_diskUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_diskUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_diskUBO);
    
    // Objects UBO
    glGenBuffers(1, &m_objectsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_objectsUBO);
    GLsizeiptr objUBOSize = sizeof(int) + 3 * sizeof(float) +  // numObjects + padding
                           16 * (sizeof(glm::vec4) + sizeof(glm::vec4) + sizeof(float));
    glBufferData(GL_UNIFORM_BUFFER, objUBOSize, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, m_objectsUBO);
    
    checkGLError("initialize UBOs");
}

void Renderer::dispatchCompute(const Camera& camera, const std::vector<Object>& objects) {
    // Determine resolution based on camera movement
    int width, height;
    if (m_adaptiveQuality && camera.isMoving()) {
        width = m_movingWidth;
        height = m_movingHeight;
    } else {
        width = m_staticWidth;
        height = m_staticHeight;
    }
    
    // Resize texture if needed
    glBindTexture(GL_TEXTURE_2D, m_rayTracingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    
    // Use compute shader
    glUseProgram(m_computeShaderProgram);
    
    // Upload uniform data
    uploadCameraUBO(camera);
    uploadDiskUBO();
    uploadObjectsUBO(objects);
    
    // Bind texture as image
    glBindImageTexture(0, m_rayTracingTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    
    // Dispatch compute shader
    GLuint groupsX = (width + 15) / 16;   // 16x16 work group size
    GLuint groupsY = (height + 15) / 16;
    glDispatchCompute(groupsX, groupsY, 1);
    
    // Memory barrier
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    
    checkGLError("dispatch compute");
}

void Renderer::uploadCameraUBO(const Camera& camera) {
    struct CameraUBOData {
        glm::vec3 pos; float _pad0;
        glm::vec3 right; float _pad1;
        glm::vec3 up; float _pad2;
        glm::vec3 forward; float _pad3;
        float tanHalfFov;
        float aspect;
        bool moving;
        int _pad4;
    } data;
    
    data.pos = camera.getPosition();
    data.right = camera.getRight();
    data.up = camera.getUp();
    data.forward = camera.getForward();
    data.tanHalfFov = std::tan(glm::radians(60.0f * 0.5f));  // Half FOV in radians
    data.aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
    data.moving = camera.isMoving();
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
}

void Renderer::uploadDiskUBO() {
    // Accretion disk parameters
    float diskData[4] = {
        m_config.getFloat("accretionDisk.innerRadius", 2.785e10f),
        m_config.getFloat("accretionDisk.outerRadius", 6.595e10f),
        m_config.getFloat("accretionDisk.temperature", 10000.0f),
        m_config.getFloat("accretionDisk.thickness", 1e9f)
    };
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_diskUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(diskData), diskData);
}

void Renderer::uploadObjectsUBO(const std::vector<Object>& objects) {
    struct ObjectsUBOData {
        int numObjects;
        float _pad0, _pad1, _pad2;
        glm::vec4 posRadius[16];
        glm::vec4 color[16];
        float mass[16];
    } data;
    
    size_t count = std::min(objects.size(), size_t(16));
    data.numObjects = static_cast<int>(count);
    
    for (size_t i = 0; i < count; ++i) {
        const auto& obj = objects[i];
        data.posRadius[i] = glm::vec4(obj.getPosition(), obj.getRadius());
        data.color[i] = obj.getColor();
        data.mass[i] = obj.getMass();
    }
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_objectsUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(data), &data);
}

void Renderer::renderGrid(const glm::mat4& viewProjMatrix) {
    // Regenerate grid with current objects (could be optimized)
    // For now, we'll use a simple static grid
    if (m_gridIndexCount == 0) {
        std::vector<Object> emptyObjects;  // Simplified for now
        generateGrid(emptyObjects);
    }
    
    glUseProgram(m_gridShaderProgram);
    
    // Set uniforms
    GLint viewProjLoc = glGetUniformLocation(m_gridShaderProgram, "viewProj");
    glUniformMatrix4fv(viewProjLoc, 1, GL_FALSE, &viewProjMatrix[0][0]);
    
    GLint timeLoc = glGetUniformLocation(m_gridShaderProgram, "time");
    if (timeLoc >= 0) {
        glUniform1f(timeLoc, glfwGetTime());
    }
    
    // Render grid
    glBindVertexArray(m_gridVAO);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_LINES, m_gridIndexCount, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    glBindVertexArray(0);
}

void Renderer::renderFullscreenQuad() {
    glUseProgram(m_quadShaderProgram);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_rayTracingTexture);
    glUniform1i(glGetUniformLocation(m_quadShaderProgram, "screenTexture"), 0);
    
    glBindVertexArray(m_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer::checkGLError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::string message = "OpenGL error in " + operation + ": ";
        switch (error) {
            case GL_INVALID_ENUM: message += "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: message += "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: message += "GL_INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY: message += "GL_OUT_OF_MEMORY"; break;
            default: message += "Unknown error " + std::to_string(error);
        }
        Logger::getInstance().log(Logger::Level::ERROR, message);
    }
}