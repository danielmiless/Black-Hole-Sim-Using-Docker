/**
 * @file Physics.cpp
 * @brief Implementation of the N-body gravitational physics system
 */

#include "Physics.h"
#include "../utils/Logger.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <vector>
#include <map>

Physics::Physics(const Config& config)
    : m_config(config)
    , m_blackHole(
        glm::vec3(
            config.getDoubleArray("blackHole.position", {0.0, 0.0, 0.0})[0],
            config.getDoubleArray("blackHole.position", {0.0, 0.0, 0.0})[1],
            config.getDoubleArray("blackHole.position", {0.0, 0.0, 0.0})[2]
        ),
        config.getDouble("blackHole.mass", 8.54e36),
        config.getString("blackHole.name", "Sagittarius A*")
    )
    , m_gravityEnabled(config.getBool("physics.enableGravity", false))
    , m_integrationMethod(IntegrationMethod::RK4)
    , m_timeStep(config.getFloat("physics.timeStep", 0.016666f))
    , m_G(config.getDouble("physics.gravityConstant", 6.67430e-11))
    , m_simulationTime(0.0)
    , m_stepCount(0) {
    
    // Parse integration method from config
    std::string methodStr = config.getString("physics.integrationMethod", "rk4");
    if (methodStr == "euler") {
        m_integrationMethod = IntegrationMethod::EULER;
    } else if (methodStr == "leapfrog") {
        m_integrationMethod = IntegrationMethod::LEAPFROG;
    } else {
        m_integrationMethod = IntegrationMethod::RK4;
    }
    
    initializeObjects();
    loadObjectsFromConfig();
    
    Logger::getInstance().log(Logger::Level::INFO, 
        "Physics system initialized with " + std::to_string(m_objects.size()) + 
        " objects, integration method: " + integrationMethodToString(m_integrationMethod));
}

void Physics::update(float deltaTime) {
    if (deltaTime <= 0.0f) return;
    
    m_simulationTime += deltaTime;
    m_stepCount++;
    
    if (m_gravityEnabled) {
        // Calculate all gravitational forces
        calculateGravitationalForces();
        applyBlackHoleGravity();
        
        // Integrate motion using selected method
        integrateMotion(deltaTime);
        
        // Handle collisions and cleanup
        handleCollisions();
        removeSwallowedObjects();
    }
    
    // Update all objects
    for (auto& object : m_objects) {
        object.updatePhysics(deltaTime);
    }
    
    // Log performance info occasionally
    if (m_stepCount % 3600 == 0) { // Every 60 seconds at 60 FPS
        Logger::getInstance().log(Logger::Level::DEBUG, 
            "Physics: " + std::to_string(m_stepCount) + " steps, " + 
            std::to_string(m_simulationTime) + "s simulated");
    }
}

void Physics::addObject(const Object& object) {
    if (m_objects.size() >= 16) {  // Limit for GPU uniform buffer
        Logger::getInstance().log(Logger::Level::WARNING, 
            "Cannot add object: maximum limit of 16 objects reached");
        return;
    }
    
    m_objects.push_back(object);
    Logger::getInstance().log(Logger::Level::INFO, 
        "Object '" + object.getName() + "' added to physics simulation");
}

void Physics::removeObject(size_t index) {
    if (index >= m_objects.size()) return;
    
    std::string name = m_objects[index].getName();
    m_objects.erase(m_objects.begin() + index);
    Logger::getInstance().log(Logger::Level::INFO, 
        "Object '" + name + "' removed from physics simulation");
}

void Physics::clearObjects() {
    size_t count = m_objects.size();
    m_objects.clear();
    Logger::getInstance().log(Logger::Level::INFO, 
        "Cleared " + std::to_string(count) + " objects from physics simulation");
}

void Physics::resetSimulation() {
    m_simulationTime = 0.0;
    m_stepCount = 0;
    
    // Reset all objects to initial state (would need to store initial conditions)
    for (auto& object : m_objects) {
        object.setVelocity(glm::vec3(0.0f));
        object.resetForces();
    }
    
    Logger::getInstance().log(Logger::Level::INFO, "Physics simulation reset");
}

void Physics::processKeyboard(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_G:
                toggleGravity();
                Logger::getInstance().log(Logger::Level::INFO, 
                    "Gravity " + std::string(m_gravityEnabled ? "enabled" : "disabled"));
                break;
                
            case GLFW_KEY_1:
                setIntegrationMethod(IntegrationMethod::EULER);
                Logger::getInstance().log(Logger::Level::INFO, "Integration method: Euler");
                break;
                
            case GLFW_KEY_2:
                setIntegrationMethod(IntegrationMethod::LEAPFROG);
                Logger::getInstance().log(Logger::Level::INFO, "Integration method: Leapfrog");
                break;
                
            case GLFW_KEY_3:
                setIntegrationMethod(IntegrationMethod::RK4);
                Logger::getInstance().log(Logger::Level::INFO, "Integration method: RK4");
                break;
                
            case GLFW_KEY_SPACE:
                resetSimulation();
                break;
        }
    }
}

double Physics::getTotalEnergy() const {
    return getKineticEnergy() + getPotentialEnergy();
}

double Physics::getKineticEnergy() const {
    double totalKE = 0.0;
    for (const auto& object : m_objects) {
        if (object.isActive()) {
            totalKE += object.getKineticEnergy();
        }
    }
    return totalKE;
}

double Physics::getPotentialEnergy() const {
    double totalPE = 0.0;
    
    // Gravitational potential energy between all pairs of objects
    for (size_t i = 0; i < m_objects.size(); ++i) {
        const auto& obj1 = m_objects[i];
        if (!obj1.isActive()) continue;
        
        // Potential energy with black hole
        double distance = glm::length(obj1.getPosition() - m_blackHole.getPosition());
        if (distance > 0.0) {
            totalPE -= m_G * m_blackHole.getMass() * obj1.getMass() / distance;
        }
        
        // Potential energy with other objects
        for (size_t j = i + 1; j < m_objects.size(); ++j) {
            const auto& obj2 = m_objects[j];
            if (!obj2.isActive()) continue;
            
            distance = obj1.getDistanceTo(obj2);
            if (distance > 0.0) {
                totalPE -= m_G * obj1.getMass() * obj2.getMass() / distance;
            }
        }
    }
    
    return totalPE;
}

void Physics::initializeObjects() {
    // Create some default test objects if none exist
    if (m_objects.empty()) {
        // Add default test objects
        Object star1(
            glm::vec3(4e11f, 0.0f, 0.0f),    // Position
            glm::vec3(0.0f, 0.0f, 0.0f),     // Velocity
            1.98892e30,                       // Mass (1 solar mass)
            4e10f,                            // Radius
            glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), // Color (yellow)
            "Test Star 1",
            Object::Type::STAR
        );
        
        Object planet1(
            glm::vec3(0.0f, 0.0f, 4e11f),    // Position
            glm::vec3(0.0f, 0.0f, 0.0f),     // Velocity
            5.972e24,                         // Mass (Earth-like)
            4e10f,                            // Radius
            glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // Color (red)
            "Test Planet 1",
            Object::Type::PLANET
        );
        
        addObject(star1);
        addObject(planet1);
    }
}

void Physics::loadObjectsFromConfig() {
    // This would load objects from the JSON configuration
    // For now, we use the default objects created in initializeObjects()
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Loading objects from configuration (using defaults for now)");
}

void Physics::calculateGravitationalForces() {
    // Calculate forces between all pairs of objects
    for (size_t i = 0; i < m_objects.size(); ++i) {
        auto& obj1 = m_objects[i];
        if (!obj1.isActive()) continue;
        
        for (size_t j = i + 1; j < m_objects.size(); ++j) {
            auto& obj2 = m_objects[j];
            if (!obj2.isActive()) continue;
            
            // Calculate gravitational force between obj1 and obj2
            glm::vec3 displacement = obj2.getPosition() - obj1.getPosition();
            double distance = glm::length(displacement);
            
            if (distance > 0.0) {
                // F = G * m1 * m2 / r²
                double forceMagnitude = m_G * obj1.getMass() * obj2.getMass() / (distance * distance);
                glm::vec3 forceDirection = glm::normalize(displacement);
                glm::vec3 force = forceDirection * static_cast<float>(forceMagnitude);
                
                // Apply equal and opposite forces (Newton's 3rd law)
                obj1.applyForce(force);
                obj2.applyForce(-force);
            }
        }
    }
}

void Physics::applyBlackHoleGravity() {
    glm::vec3 blackHolePos = m_blackHole.getPosition();
    double blackHoleMass = m_blackHole.getMass();
    
    for (auto& object : m_objects) {
        if (!object.isActive()) continue;
        
        glm::vec3 displacement = blackHolePos - object.getPosition();
        double distance = glm::length(displacement);
        
        // Don't apply force if too close (avoid singularity)
        double minDistance = m_blackHole.getSchwarzschildRadius() * 0.1;
        if (distance > minDistance) {
            // F = G * M * m / r²
            double forceMagnitude = m_G * blackHoleMass * object.getMass() / (distance * distance);
            glm::vec3 forceDirection = glm::normalize(displacement);
            glm::vec3 force = forceDirection * static_cast<float>(forceMagnitude);
            
            object.applyForce(force);
        }
    }
}

void Physics::integrateMotion(float deltaTime) {
    switch (m_integrationMethod) {
        case IntegrationMethod::EULER:
            integrateEuler(deltaTime);
            break;
        case IntegrationMethod::LEAPFROG:
            integrateLeapfrog(deltaTime);
            break;
        case IntegrationMethod::RK4:
            integrateRK4(deltaTime);
            break;
    }
}

void Physics::integrateEuler(float deltaTime) {
    // Simple Euler integration (handled by Object::updatePhysics)
    // This is the default behavior, so we don't need to do anything special here
}

void Physics::integrateLeapfrog(float deltaTime) {
    // Leapfrog integration - better for orbital mechanics
    // For now, fallback to Euler (could be improved)
    integrateEuler(deltaTime);
}

void Physics::integrateRK4(float deltaTime) {
    // Runge-Kutta 4th order - most accurate but slower
    // For now, fallback to Euler (could be improved with proper RK4 implementation)
    integrateEuler(deltaTime);
}

void Physics::handleCollisions() {
    for (size_t i = 0; i < m_objects.size(); ++i) {
        auto& obj1 = m_objects[i];
        if (!obj1.isActive()) continue;
        
        for (size_t j = i + 1; j < m_objects.size(); ++j) {
            auto& obj2 = m_objects[j];
            if (!obj2.isActive()) continue;
            
            if (obj1.isCollidingWith(obj2)) {
                Logger::getInstance().log(Logger::Level::INFO, 
                    "Collision detected between '" + obj1.getName() + "' and '" + obj2.getName() + "'");
                
                // Simple collision response - merge objects
                if (obj1.getMass() >= obj2.getMass()) {
                    // obj1 absorbs obj2
                    obj2.setActive(false);
                } else {
                    // obj2 absorbs obj1
                    obj1.setActive(false);
                }
            }
        }
    }
}

void Physics::removeSwallowedObjects() {
    auto it = m_objects.begin();
    while (it != m_objects.end()) {
        if (it->isActive() && m_blackHole.isInsideEventHorizon(it->getPosition())) {
            Logger::getInstance().log(Logger::Level::INFO, 
                "Object '" + it->getName() + "' crossed the event horizon and was absorbed");
            it = m_objects.erase(it);
        } else if (!it->isActive()) {
            Logger::getInstance().log(Logger::Level::DEBUG, 
                "Removing inactive object '" + it->getName() + "'");
            it = m_objects.erase(it);
        } else {
            ++it;
        }
    }
}

glm::vec3 Physics::calculateAcceleration(size_t objectIndex) const {
    if (objectIndex >= m_objects.size()) return glm::vec3(0.0f);
    
    const auto& object = m_objects[objectIndex];
    glm::vec3 totalAcceleration(0.0f);
    
    // Acceleration from black hole
    glm::vec3 displacement = m_blackHole.getPosition() - object.getPosition();
    double distance = glm::length(displacement);
    
    if (distance > m_blackHole.getSchwarzschildRadius() * 0.1) {
        double acceleration = m_G * m_blackHole.getMass() / (distance * distance);
        glm::vec3 direction = glm::normalize(displacement);
        totalAcceleration += direction * static_cast<float>(acceleration);
    }
    
    // Acceleration from other objects
    for (size_t i = 0; i < m_objects.size(); ++i) {
        if (i == objectIndex || !m_objects[i].isActive()) continue;
        
        const auto& other = m_objects[i];
        displacement = other.getPosition() - object.getPosition();
        distance = glm::length(displacement);
        
        if (distance > 0.0) {
            double acceleration = m_G * other.getMass() / (distance * distance);
            glm::vec3 direction = glm::normalize(displacement);
            totalAcceleration += direction * static_cast<float>(acceleration);
        }
    }
    
    return totalAcceleration;
}

std::string Physics::integrationMethodToString(IntegrationMethod method) const {
    switch (method) {
        case IntegrationMethod::EULER: return "Euler";
        case IntegrationMethod::LEAPFROG: return "Leapfrog";
        case IntegrationMethod::RK4: return "Runge-Kutta 4";
        default: return "Unknown";
    }
}