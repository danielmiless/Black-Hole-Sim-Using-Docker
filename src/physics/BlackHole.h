/**
 * @file BlackHole.h
 * @brief Black hole physics implementation with Schwarzschild metric
 */

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <GLFW/glfw3.h>

/**
 * @brief Represents a black hole with accurate relativistic physics
 * 
 * This class implements the Schwarzschild solution to Einstein's field equations,
 * providing accurate calculations for event horizon, gravitational effects,
 * and spacetime curvature around a non-rotating black hole.
 */
class BlackHole {
public:
    /**
     * @brief Construct a black hole
     * @param position Position in 3D space
     * @param mass Mass in kilograms
     * @param name Optional name for the black hole
     */
    BlackHole(const glm::vec3& position, double mass, const std::string& name = "Unnamed");
    
    /**
     * @brief Get the black hole's position
     * @return Position vector
     */
    glm::vec3 getPosition() const { return m_position; }
    
    /**
     * @brief Set the black hole's position
     * @param position New position
     */
    void setPosition(const glm::vec3& position) { m_position = position; }
    
    /**
     * @brief Get the black hole's mass
     * @return Mass in kilograms
     */
    double getMass() const { return m_mass; }
    
    /**
     * @brief Set the black hole's mass
     * @param mass New mass in kilograms
     */
    void setMass(double mass);
    
    /**
     * @brief Get the Schwarzschild radius (event horizon)
     * @return Event horizon radius in meters
     */
    double getSchwarzschildRadius() const { return m_schwarzschildRadius; }
    
    /**
     * @brief Get the photon sphere radius
     * @return Photon sphere radius (1.5 * Rs)
     */
    double getPhotonSphereRadius() const { return m_schwarzschildRadius * 1.5; }
    
    /**
     * @brief Get the innermost stable circular orbit (ISCO)
     * @return ISCO radius (3 * Rs for Schwarzschild)
     */
    double getISCORadius() const { return m_schwarzschildRadius * 3.0; }
    
    /**
     * @brief Get the black hole's name
     * @return Name string
     */
    std::string getName() const { return m_name; }
    
    /**
     * @brief Check if a point is inside the event horizon
     * @param point Point to test
     * @return true if inside event horizon
     */
    bool isInsideEventHorizon(const glm::vec3& point) const;
    
    /**
     * @brief Check if a point is within the photon sphere
     * @param point Point to test
     * @return true if inside photon sphere
     */
    bool isInsidePhotonSphere(const glm::vec3& point) const;
    
    /**
     * @brief Calculate gravitational acceleration at a point
     * @param point Point in space
     * @return Acceleration vector (Newtonian approximation)
     */
    glm::vec3 getGravitationalAcceleration(const glm::vec3& point) const;
    
    /**
     * @brief Calculate gravitational potential at a point
     * @param point Point in space
     * @return Gravitational potential (negative)
     */
    double getGravitationalPotential(const glm::vec3& point) const;
    
    /**
     * @brief Calculate the metric coefficient g_tt at a radius
     * @param radius Distance from black hole center
     * @return Time metric coefficient (1 - Rs/r)
     */
    double getMetricCoefficient(double radius) const;
    
    /**
     * @brief Calculate gravitational time dilation factor
     * @param radius Distance from black hole center
     * @return Time dilation factor (proper time / coordinate time)
     */
    double getTimeDilationFactor(double radius) const;
    
    /**
     * @brief Calculate spacetime curvature for grid visualization
     * @param point Point in spacetime
     * @return Vertical displacement for grid warping
     */
    float getSpacetimeCurvature(const glm::vec3& point) const;
    
    /**
     * @brief Get surface temperature (Hawking radiation)
     * @return Temperature in Kelvin
     */
    double getHawkingTemperature() const;
    
    /**
     * @brief Get black hole entropy (Bekenstein-Hawking entropy)
     * @return Entropy in natural units
     */
    double getBekensteinHawkingEntropy() const;

private:
    glm::vec3 m_position;           ///< Position in 3D space
    double m_mass;                  ///< Mass in kilograms
    double m_schwarzschildRadius;   ///< Event horizon radius
    std::string m_name;             ///< Black hole name
    
    // Physical constants
    static constexpr double G = 6.67430e-11;       ///< Gravitational constant
    static constexpr double c = 299792458.0;        ///< Speed of light
    static constexpr double hbar = 1.054571817e-34; ///< Reduced Planck constant
    static constexpr double k_B = 1.380649e-23;     ///< Boltzmann constant
    
    /**
     * @brief Calculate Schwarzschild radius from mass
     */
    void updateSchwarzschildRadius();
};

/**
 * @brief Common black hole masses for reference
 */
namespace BlackHoleMasses {
    constexpr double SAGITTARIUS_A_STAR = 8.54e36;      // ~4.3 million solar masses
    constexpr double STELLAR_MASS = 2.0e31;             // ~10 solar masses (typical stellar BH)
    constexpr double INTERMEDIATE_MASS = 2.0e35;        // ~100,000 solar masses
    constexpr double SUPERMASSIVE = 2.0e39;             // ~1 billion solar masses
    constexpr double SOLAR_MASS = 1.98892e30;           // One solar mass
}