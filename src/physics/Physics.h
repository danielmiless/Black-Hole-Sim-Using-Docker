/**
 * @file Physics.h
 * @brief Physics simulation system for N-body gravitational interactions
 */

#pragma once

#include <vector>
#include <memory>
#include "../objects/Object.h"
#include "../utils/Config.h"
#include "BlackHole.h"
#include <string>
#include <map>
#include <GLFW/glfw3.h>

/**
 * @brief Manages the physics simulation including gravity and object interactions
 * 
 * This class handles N-body gravitational simulations, integrates object motion,
 * and manages the central black hole. It supports different integration methods
 * and can toggle gravity effects on/off for visualization purposes.
 */
class Physics {
public:
    /**
     * @brief Integration methods for physics simulation
     */
    enum class IntegrationMethod {
        EULER,          ///< Simple Euler integration (fast, less accurate)
        LEAPFROG,       ///< Leapfrog integration (better for orbital mechanics)
        RK4             ///< Runge-Kutta 4th order (accurate, slower)
    };
    
    /**
     * @brief Construct physics system
     * @param config Configuration object
     */
    explicit Physics(const Config& config);
    
    /**
     * @brief Update physics simulation
     * @param deltaTime Time step in seconds
     */
    void update(float deltaTime);
    
    /**
     * @brief Get list of all objects in the simulation
     * @return Reference to object list
     */
    const std::vector<Object>& getObjects() const { return m_objects; }
    
    /**
     * @brief Get the central black hole
     * @return Reference to the black hole
     */
    const BlackHole& getBlackHole() const { return m_blackHole; }
    
    /**
     * @brief Get non-const reference to black hole (for modifications)
     * @return Reference to the black hole
     */
    BlackHole& getBlackHole() { return m_blackHole; }
    
    /**
     * @brief Add an object to the simulation
     * @param object Object to add
     */
    void addObject(const Object& object);
    
    /**
     * @brief Remove an object from the simulation
     * @param index Index of object to remove
     */
    void removeObject(size_t index);
    
    /**
     * @brief Clear all objects from the simulation
     */
    void clearObjects();
    
    /**
     * @brief Enable or disable gravity simulation
     * @param enabled True to enable gravity
     */
    void setGravityEnabled(bool enabled) { m_gravityEnabled = enabled; }
    
    /**
     * @brief Check if gravity is currently enabled
     * @return True if gravity is enabled
     */
    bool isGravityEnabled() const { return m_gravityEnabled; }
    
    /**
     * @brief Toggle gravity on/off
     */
    void toggleGravity() { m_gravityEnabled = !m_gravityEnabled; }
    
    /**
     * @brief Set integration method
     * @param method Integration method to use
     */
    void setIntegrationMethod(IntegrationMethod method) { m_integrationMethod = method; }
    
    /**
     * @brief Get current integration method
     * @return Current integration method
     */
    IntegrationMethod getIntegrationMethod() const { return m_integrationMethod; }
    
    /**
     * @brief Reset all objects to their initial positions and velocities
     */
    void resetSimulation();
    
    /**
     * @brief Process keyboard input for physics controls
     * @param key GLFW key code
     * @param action GLFW action (press, release, repeat)
     * @param mods GLFW modifier keys
     */
    void processKeyboard(int key, int action, int mods);
    
    /**
     * @brief Get total energy of the system (kinetic + potential)
     * @return Total energy in Joules
     */
    double getTotalEnergy() const;
    
    /**
     * @brief Get total kinetic energy
     * @return Kinetic energy in Joules
     */
    double getKineticEnergy() const;
    
    /**
     * @brief Get total gravitational potential energy
     * @return Potential energy in Joules
     */
    double getPotentialEnergy() const;

private:
    Config m_config;                        ///< Configuration settings
    std::vector<Object> m_objects;          ///< List of objects in simulation
    BlackHole m_blackHole;                  ///< Central black hole
    
    // Physics parameters
    bool m_gravityEnabled;                  ///< Is gravity simulation enabled?
    IntegrationMethod m_integrationMethod; ///< Numerical integration method
    float m_timeStep;                       ///< Physics time step
    double m_G;                             ///< Gravitational constant
    
    // Performance tracking
    double m_simulationTime;                ///< Total simulation time elapsed
    size_t m_stepCount;                     ///< Number of simulation steps
    
    /**
     * @brief Initialize physics system with default objects
     */
    void initializeObjects();
    
    /**
     * @brief Load objects from configuration
     */
    void loadObjectsFromConfig();
    
    /**
     * @brief Calculate gravitational forces between all objects
     */
    void calculateGravitationalForces();
    
    /**
     * @brief Apply gravitational force from black hole to objects
     */
    void applyBlackHoleGravity();
    
    /**
     * @brief Integrate object motion using selected method
     * @param deltaTime Time step
     */
    void integrateMotion(float deltaTime);
    
    /**
     * @brief Euler integration step
     * @param deltaTime Time step
     */
    void integrateEuler(float deltaTime);
    
    /**
     * @brief Leapfrog integration step
     * @param deltaTime Time step
     */
    void integrateLeapfrog(float deltaTime);
    
    /**
     * @brief Runge-Kutta 4th order integration step
     * @param deltaTime Time step
     */
    void integrateRK4(float deltaTime);
    
    /**
     * @brief Check for collisions and handle them
     */
    void handleCollisions();
    
    /**
     * @brief Remove objects that have fallen into the black hole
     */
    void removeSwallowedObjects();
    
    /**
     * @brief Calculate gravitational acceleration on an object
     * @param objectIndex Index of the object
     * @return Acceleration vector
     */
    glm::vec3 calculateAcceleration(size_t objectIndex) const;
    
    /**
     * @brief Get string representation of integration method
     * @param method Integration method
     * @return Method name as string
     */
    std::string integrationMethodToString(IntegrationMethod method) const;
};