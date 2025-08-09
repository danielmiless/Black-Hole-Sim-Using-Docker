/**
 * @file Object.h
 * @brief Generic celestial object representation
 */

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

/**
 * @brief Represents a celestial object in the simulation
 * 
 * This class encapsulates the properties and behavior of objects like stars,
 * planets, asteroids, or any other massive body that can interact gravitationally
 * with the black hole and other objects.
 */
class Object {
public:
    /**
     * @brief Object types for different rendering and physics behavior
     */
    enum class Type {
        STAR,           ///< Stellar object with emission
        PLANET,         ///< Planetary body
        ASTEROID,       ///< Small rocky body
        GAS_CLOUD,      ///< Diffuse gas cloud
        DEBRIS,         ///< Space debris
        TEST_MASS       ///< Massless test particle for visualization
    };
    
    /**
     * @brief Construct an object
     * @param position Initial position in 3D space
     * @param velocity Initial velocity vector
     * @param mass Mass in kilograms
     * @param radius Physical radius in meters
     * @param color RGBA color for rendering
     * @param name Optional name for the object
     * @param type Object type (default: PLANET)
     */
    Object(const glm::vec3& position, 
           const glm::vec3& velocity, 
           double mass, 
           float radius, 
           const glm::vec4& color,
           const std::string& name = "Unnamed",
           Type type = Type::PLANET);
    
    // Getters
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getVelocity() const { return m_velocity; }
    glm::vec3 getAcceleration() const { return m_acceleration; }
    double getMass() const { return m_mass; }
    float getRadius() const { return m_radius; }
    glm::vec4 getColor() const { return m_color; }
    std::string getName() const { return m_name; }
    Type getType() const { return m_type; }
    bool isActive() const { return m_active; }
    
    // Setters
    void setPosition(const glm::vec3& position) { m_position = position; }
    void setVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
    void setAcceleration(const glm::vec3& acceleration) { m_acceleration = acceleration; }
    void setMass(double mass) { m_mass = mass; }
    void setRadius(float radius) { m_radius = radius; }
    void setColor(const glm::vec4& color) { m_color = color; }
    void setName(const std::string& name) { m_name = name; }
    void setType(Type type) { m_type = type; }
    void setActive(bool active) { m_active = active; }
    
    /**
     * @brief Update object physics (integrate motion)
     * @param deltaTime Time step in seconds
     */
    void updatePhysics(float deltaTime);
    
    /**
     * @brief Apply a force to the object
     * @param force Force vector in Newtons
     */
    void applyForce(const glm::vec3& force);
    
    /**
     * @brief Reset accumulated forces (call after integration)
     */
    void resetForces();
    
    /**
     * @brief Get distance to another object
     * @param other Other object
     * @return Distance in meters
     */
    double getDistanceTo(const Object& other) const;
    
    /**
     * @brief Check collision with another object
     * @param other Other object
     * @return True if objects are colliding
     */
    bool isCollidingWith(const Object& other) const;
    
    /**
     * @brief Get kinetic energy
     * @return Kinetic energy in Joules
     */
    double getKineticEnergy() const;
    
    /**
     * @brief Get orbital velocity for circular orbit at current position
     * @param centralMass Mass of central object (e.g., black hole)
     * @return Orbital velocity magnitude
     */
    double getOrbitalVelocity(double centralMass) const;
    
    /**
     * @brief Set velocity for circular orbit around a central mass
     * @param centralMass Mass of central object
     * @param centralPosition Position of central object
     */
    void setCircularOrbit(double centralMass, const glm::vec3& centralPosition);
    
    /**
     * @brief Get object info as formatted string
     * @return Formatted string with object properties
     */
    std::string getInfoString() const;
    
    /**
     * @brief Create object from JSON-like configuration
     * @param config Configuration values
     * @return Created object
     */
    static Object fromConfig(const std::vector<double>& position,
                           const std::vector<double>& velocity,
                           double mass,
                           float radius,
                           const std::vector<float>& color,
                           const std::string& name = "Unnamed");

private:
    // Physical properties
    glm::vec3 m_position;       ///< Position in 3D space (meters)
    glm::vec3 m_velocity;       ///< Velocity vector (m/s)
    glm::vec3 m_acceleration;   ///< Current acceleration (m/s²)
    double m_mass;              ///< Mass (kg)
    float m_radius;             ///< Physical radius (meters)
    
    // Visual properties
    glm::vec4 m_color;          ///< RGBA color for rendering
    std::string m_name;         ///< Object name
    Type m_type;                ///< Object type
    
    // State
    bool m_active;              ///< Is object active in simulation?
    glm::vec3 m_forceSum;       ///< Accumulated forces for this frame
    
    // History tracking (for trails, etc.)
    std::vector<glm::vec3> m_positionHistory;  ///< Recent positions
    size_t m_maxHistorySize;                   ///< Maximum history entries
    
    /**
     * @brief Update position history
     */
    void updateHistory();
};