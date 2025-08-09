/**
 * @file BlackHole.cpp
 * @brief Implementation of black hole physics
 */

#include "BlackHole.h"
#include "../utils/Logger.h"
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

BlackHole::BlackHole(const glm::vec3& position, double mass, const std::string& name)
    : m_position(position)
    , m_mass(mass)
    , m_schwarzschildRadius(0.0)
    , m_name(name) {
    
    updateSchwarzschildRadius();
    
    Logger::getInstance().log(Logger::Level::INFO, 
        "Black hole '" + m_name + "' created with mass " + std::to_string(mass) + 
        " kg, Rs = " + std::to_string(m_schwarzschildRadius) + " m");
}

void BlackHole::setMass(double mass) {
    m_mass = mass;
    updateSchwarzschildRadius();
    
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Black hole '" + m_name + "' mass updated: " + std::to_string(mass) + " kg");
}

bool BlackHole::isInsideEventHorizon(const glm::vec3& point) const {
    double distance = glm::length(point - m_position);
    return distance <= m_schwarzschildRadius;
}

bool BlackHole::isInsidePhotonSphere(const glm::vec3& point) const {
    double distance = glm::length(point - m_position);
    return distance <= getPhotonSphereRadius();
}

glm::vec3 BlackHole::getGravitationalAcceleration(const glm::vec3& point) const {
    glm::vec3 r_vec = point - m_position;
    double r = glm::length(r_vec);
    
    // Avoid singularity at r = 0
    if (r < m_schwarzschildRadius * 0.1) {
        return glm::vec3(0.0f);  // Inside deep gravitational well
    }
    
    // Newtonian approximation: a = -GM/r² * r̂
    double acceleration_magnitude = G * m_mass / (r * r);
    glm::vec3 direction = -glm::normalize(r_vec);  // Point toward black hole
    
    return direction * static_cast<float>(acceleration_magnitude);
}

double BlackHole::getGravitationalPotential(const glm::vec3& point) const {
    double r = glm::length(point - m_position);
    
    // Avoid singularity
    if (r < m_schwarzschildRadius * 0.1) {
        r = m_schwarzschildRadius * 0.1;
    }
    
    // Newtonian potential: Φ = -GM/r
    return -G * m_mass / r;
}

double BlackHole::getMetricCoefficient(double radius) const {
    // Schwarzschild metric coefficient g_tt = 1 - Rs/r
    if (radius <= m_schwarzschildRadius) {
        return 0.0;  // At or inside event horizon
    }
    
    return 1.0 - (m_schwarzschildRadius / radius);
}

double BlackHole::getTimeDilationFactor(double radius) const {
    // Time dilation factor: dt_proper/dt_coordinate = sqrt(g_tt)
    double g_tt = getMetricCoefficient(radius);
    
    if (g_tt <= 0.0) {
        return 0.0;  // Time stops at event horizon
    }
    
    return std::sqrt(g_tt);
}

float BlackHole::getSpacetimeCurvature(const glm::vec3& point) const {
    double r = glm::length(point - m_position);
    
    if (r <= m_schwarzschildRadius) {
        // Inside event horizon - extreme curvature
        return static_cast<float>(2.0 * std::sqrt(m_schwarzschildRadius * m_schwarzschildRadius)) - 3e10f;
    }
    
    // Outside event horizon - gravitational potential creates curvature
    // This is a visualization approximation, not the actual spacetime metric
    double curvature = 2.0 * std::sqrt(m_schwarzschildRadius * (r - m_schwarzschildRadius));
    return static_cast<float>(curvature) - 3e10f;
}

double BlackHole::getHawkingTemperature() const {
    // Hawking temperature: T = ℏc³/(8πGMk_B)
    return (hbar * c * c * c) / (8.0 * M_PI * G * m_mass * k_B);
}

double BlackHole::getBekensteinHawkingEntropy() const {
    // Bekenstein-Hawking entropy: S = (k_B * c³ * A)/(4ℏG)
    // where A = 4πRs² is the area of the event horizon
    double area = 4.0 * M_PI * m_schwarzschildRadius * m_schwarzschildRadius;
    return (k_B * c * c * c * area) / (4.0 * hbar * G);
}

void BlackHole::updateSchwarzschildRadius() {
    // Schwarzschild radius: Rs = 2GM/c²
    m_schwarzschildRadius = (2.0 * G * m_mass) / (c * c);
    
    Logger::getInstance().log(Logger::Level::DEBUG, 
        "Schwarzschild radius updated: " + std::to_string(m_schwarzschildRadius) + " m");
}