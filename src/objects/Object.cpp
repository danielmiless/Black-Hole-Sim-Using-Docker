/**
 * @file Object.cpp
 * @brief Implementation of the celestial object class
 */

#include "Object.h"
#include "../utils/Logger.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

Object::Object(const glm::vec3& position, 
               const glm::vec3& velocity, 
               double mass, 
               float radius, 
               const glm::vec4& color,
               const std::string& name,
               Type type)
    : m_position(position)
    , m_velocity(velocity)
    , m_acceleration(0.0f)
    , m_mass(mass)
    , m_radius(radius)
    , m_color(color)
    , m_name(name)
    , m_type(type)
    , m_active(true)
    , m_forceSum(0.0f)
    , m_maxHistorySize(100) {
    
    m_positionHistory.reserve(m_maxHistorySize);
    m_positionHistory.push_back(position);
    
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Object '" + m_name + "' created at position (" + 
        std::to_string(position.x) + ", " + std::to_string(position.y) + ", " + std::to_string(position.z) + ")");
}

void Object::updatePhysics(float deltaTime) {
    if (!m_active || deltaTime <= 0.0f) return;
    
    // Calculate acceleration from accumulated forces (F = ma, so a = F/m)
    if (m_mass > 0.0) {
        m_acceleration = m_forceSum / static_cast<float>(m_mass);
    } else {
        m_acceleration = glm::vec3(0.0f);  // Massless objects don't accelerate
    }
    
    // Integrate velocity (v = v₀ + at)
    m_velocity += m_acceleration * deltaTime;
    
    // Integrate position (x = x₀ + vt)
    m_position += m_velocity * deltaTime;
    
    // Update position history for trails
    updateHistory();
    
    // Reset forces for next frame
    resetForces();
}

void Object::applyForce(const glm::vec3& force) {
    m_forceSum += force;
}

void Object::resetForces() {
    m_forceSum = glm::vec3(0.0f);
}

double Object::getDistanceTo(const Object& other) const {
    glm::vec3 displacement = other.m_position - m_position;
    return static_cast<double>(glm::length(displacement));
}

bool Object::isCollidingWith(const Object& other) const {
    if (!m_active || !other.m_active) return false;
    
    double distance = getDistanceTo(other);
    double combinedRadii = static_cast<double>(m_radius + other.m_radius);
    
    return distance <= combinedRadii;
}

double Object::getKineticEnergy() const {
    if (m_mass <= 0.0) return 0.0;
    
    double speedSquared = static_cast<double>(glm::dot(m_velocity, m_velocity));
    return 0.5 * m_mass * speedSquared;
}

double Object::getOrbitalVelocity(double centralMass) const {
    const double G = 6.67430e-11;  // Gravitational constant
    double radius = static_cast<double>(glm::length(m_position));
    
    if (radius <= 0.0 || centralMass <= 0.0) return 0.0;
    
    // Circular orbital velocity: v = sqrt(GM/r)
    return std::sqrt(G * centralMass / radius);
}

void Object::setCircularOrbit(double centralMass, const glm::vec3& centralPosition) {
    glm::vec3 displacement = m_position - centralPosition;
    double radius = static_cast<double>(glm::length(displacement));
    
    if (radius <= 0.0 || centralMass <= 0.0) {
        m_velocity = glm::vec3(0.0f);
        return;
    }
    
    // Calculate orbital velocity magnitude
    double orbitalSpeed = getOrbitalVelocity(centralMass);
    
    // Create perpendicular velocity vector (cross product with up vector)
    glm::vec3 radialDirection = glm::normalize(displacement);
    glm::vec3 upVector(0.0f, 1.0f, 0.0f);
    
    // If radial direction is parallel to up vector, use right vector instead
    if (std::abs(glm::dot(radialDirection, upVector)) > 0.99f) {
        upVector = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    
    glm::vec3 orbitalDirection = glm::normalize(glm::cross(radialDirection, upVector));
    m_velocity = orbitalDirection * static_cast<float>(orbitalSpeed);
    
    Logger::getInstance().log(Logger::Level::INFO, 
        "Object '" + m_name + "' set to circular orbit with velocity " + std::to_string(orbitalSpeed) + " m/s");
}

std::string Object::getInfoString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    ss << "Object: " << m_name << "\n";
    ss << "Type: ";
    switch (m_type) {
        case Type::STAR: ss << "Star"; break;
        case Type::PLANET: ss << "Planet"; break;
        case Type::ASTEROID: ss << "Asteroid"; break;
        case Type::GAS_CLOUD: ss << "Gas Cloud"; break;
        case Type::DEBRIS: ss << "Debris"; break;
        case Type::TEST_MASS: ss << "Test Mass"; break;
    }
    ss << "\n";
    
    ss << "Position: (" << m_position.x/1e9 << ", " << m_position.y/1e9 << ", " << m_position.z/1e9 << ") Gm\n";
    ss << "Velocity: (" << m_velocity.x/1e3 << ", " << m_velocity.y/1e3 << ", " << m_velocity.z/1e3 << ") km/s\n";
    ss << "Mass: " << m_mass/1.98892e30 << " solar masses\n";
    ss << "Radius: " << m_radius/1e9 << " Gm\n";
    ss << "Kinetic Energy: " << getKineticEnergy()/1e30 << " × 10³⁰ J\n";
    ss << "Active: " << (m_active ? "Yes" : "No");
    
    return ss.str();
}

Object Object::fromConfig(const std::vector<double>& position,
                         const std::vector<double>& velocity,
                         double mass,
                         float radius,
                         const std::vector<float>& color,
                         const std::string& name) {
    
    // Convert arrays to glm vectors
    glm::vec3 pos(0.0f);
    if (position.size() >= 3) {
        pos = glm::vec3(position[0], position[1], position[2]);
    }
    
    glm::vec3 vel(0.0f);
    if (velocity.size() >= 3) {
        vel = glm::vec3(velocity[0], velocity[1], velocity[2]);
    }
    
    glm::vec4 col(1.0f);
    if (color.size() >= 4) {
        col = glm::vec4(color[0], color[1], color[2], color[3]);
    } else if (color.size() >= 3) {
        col = glm::vec4(color[0], color[1], color[2], 1.0f);
    }
    
    return Object(pos, vel, mass, radius, col, name);
}

void Object::updateHistory() {
    m_positionHistory.push_back(m_position);
    
    // Limit history size to prevent memory growth
    if (m_positionHistory.size() > m_maxHistorySize) {
        m_positionHistory.erase(m_positionHistory.begin());
    }
}