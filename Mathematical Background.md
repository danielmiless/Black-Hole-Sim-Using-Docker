# Mathematical Foundation of the 3D Black Hole Simulation

*A Comprehensive Analysis of the Gravitational Physics, Spacetime Geometry, and Numerical Methods*

---

## Table of Contents

1. [Introduction](#introduction)
2. [Fundamental Physics](#fundamental-physics)
3. [The Schwarzschild Solution](#the-schwarzschild-solution)
4. [Geodesic Equations and Ray Tracing](#geodesic-equations-and-ray-tracing)
5. [Gravitational Lensing](#gravitational-lensing)
6. [N-Body Gravitational Dynamics](#n-body-gravitational-dynamics)
7. [Numerical Integration Methods](#numerical-integration-methods)
8. [Relativistic Effects](#relativistic-effects)
9. [Computational Implementation](#computational-implementation)
10. [Physical Constants and Units](#physical-constants-and-units)
11. [References](#references)

---

## Introduction

This black hole simulation implements a scientifically accurate representation of spacetime curvature around a massive compact object, specifically modeling **Sagittarius A*** - the supermassive black hole at the center of our galaxy. The simulation combines classical Newtonian mechanics with relativistic effects from Einstein's General Theory of Relativity.

The mathematical foundation spans multiple domains:
- **General Relativity**: Schwarzschild metric and geodesic equations
- **Classical Mechanics**: N-body gravitational interactions
- **Numerical Analysis**: Integration methods and computational algorithms
- **Computer Graphics**: Ray tracing through curved spacetime

---

## Fundamental Physics

### Einstein Field Equations

The simulation is rooted in Einstein's field equations, which describe the curvature of spacetime:

$$G_{\mu\nu} = \frac{8\pi G}{c^4} T_{\mu\nu}$$

Where:
- $G_{\mu\nu}$ is the Einstein tensor (spacetime curvature)
- $T_{\mu\nu}$ is the stress-energy tensor (matter and energy distribution)
- $G = 6.67430 \times 10^{-11} \, \text{m}^3 \text{kg}^{-1} \text{s}^{-2}$ (gravitational constant)
- $c = 299,792,458 \, \text{m/s}$ (speed of light)

For a spherically symmetric, non-rotating massive object in vacuum, this reduces to the **Schwarzschild solution**.

### The Equivalence Principle

The simulation implements the **weak equivalence principle**: gravitational acceleration is locally indistinguishable from acceleration in flat spacetime. This principle underlies the geodesic motion of test particles and photons.

---

## The Schwarzschild Solution

### Schwarzschild Metric

For a spherically symmetric black hole of mass $M$, the spacetime metric is:

$$ds^2 = -\left(1 - \frac{r_s}{r}\right)c^2dt^2 + \left(1 - \frac{r_s}{r}\right)^{-1}dr^2 + r^2d\theta^2 + r^2\sin^2\theta \, d\phi^2$$

Where the **Schwarzschild radius** (event horizon) is:

$$r_s = \frac{2GM}{c^2}$$

### Implementation in BlackHole.cpp

```cpp
void BlackHole::updateSchwarzschildRadius() {
    // Schwarzschild radius: Rs = 2GM/c²
    m_schwarzschildRadius = (2.0 * G * m_mass) / (c * c);
}
```

### Key Radii

The simulation calculates several critical radii:

1. **Event Horizon**: $r_s = \frac{2GM}{c^2}$
2. **Photon Sphere**: $r_{ph} = \frac{3GM}{c^2} = 1.5 r_s$
3. **Innermost Stable Circular Orbit (ISCO)**: $r_{ISCO} = 6GM/c^2 = 3r_s$

```cpp
double getPhotonSphereRadius() const { return m_schwarzschildRadius * 1.5; }
double getISCORadius() const { return m_schwarzschildRadius * 3.0; }
```

### Sagittarius A* Parameters

For the supermassive black hole at our galaxy's center:
- **Mass**: $M_{Sgr A*} = 8.54 \times 10^{36} \, \text{kg} \approx 4.3 \times 10^6 M_{\odot}$
- **Schwarzschild Radius**: $r_s \approx 1.27 \times 10^{10} \, \text{m} \approx 0.08 \, \text{AU}$

---

## Geodesic Equations and Ray Tracing

### The Geodesic Equation

Particles and photons follow geodesics - the straightest possible paths through curved spacetime. The geodesic equation is:

$$\frac{d^2x^\mu}{d\tau^2} + \Gamma^\mu_{\nu\rho}\frac{dx^\nu}{d\tau}\frac{dx^\rho}{d\tau} = 0$$

Where $\Gamma^\mu_{\nu\rho}$ are the Christoffel symbols encoding spacetime curvature.

### Christoffel Symbols for Schwarzschild Metric

The non-zero Christoffel symbols are:

$$\Gamma^r_{tt} = \frac{GM(r-2GM/c^2)}{r^2c^2(r-2GM/c^2)}$$

$$\Gamma^t_{tr} = \Gamma^t_{rt} = \frac{GM}{r^2c^2(r-2GM/c^2)}$$

$$\Gamma^r_{rr} = -\frac{GM/c^2}{r(r-2GM/c^2)}$$

$$\Gamma^r_{\theta\theta} = -(r-2GM/c^2)$$

$$\Gamma^r_{\phi\phi} = -(r-2GM/c^2)\sin^2\theta$$

$$\Gamma^\theta_{r\theta} = \Gamma^\theta_{\theta r} = \frac{1}{r}$$

$$\Gamma^\theta_{\phi\phi} = -\sin\theta\cos\theta$$

$$\Gamma^\phi_{r\phi} = \Gamma^\phi_{\phi r} = \frac{1}{r}$$

$$\Gamma^\phi_{\theta\phi} = \Gamma^\phi_{\phi\theta} = \cot\theta$$

### Simplified Implementation

For computational efficiency, the simulation uses a simplified acceleration calculation:

```cpp
glm::vec3 BlackHole::getGravitationalAcceleration(const glm::vec3& point) const {
    glm::vec3 r_vec = point - m_position;
    double r = glm::length(r_vec);
    
    // Avoid singularity at r = 0
    if (r < m_schwarzschildRadius * 0.1) {
        return glm::vec3(0.0f);
    }
    
    // Newtonian approximation: a = -GM/r² * r̂
    double acceleration_magnitude = G * m_mass / (r * r);
    glm::vec3 direction = -glm::normalize(r_vec);
    
    return direction * static_cast<float>(acceleration_magnitude);
}
```

### Compute Shader Ray Tracing

The `geodesic.comp` compute shader implements full geodesic ray tracing using the Schwarzschild metric. Each ray is integrated through spacetime following:

$$\frac{d^2\vec{r}}{dt^2} = -\frac{GM}{|\vec{r}|^3}\vec{r}\left(1 + \frac{3r_s}{4r}\right)$$

This includes the leading-order relativistic correction to Newtonian gravity.

---

## Gravitational Lensing

### Light Deflection Angle

For a photon passing a black hole at impact parameter $b$, the deflection angle is:

$$\Delta\phi = \frac{4GM}{c^2 b}$$

For $b \to \frac{3GM}{c^2}$ (photon sphere), $\Delta\phi \to \infty$ - photons can orbit indefinitely.

### Implementation in Compute Shader

The ray tracing shader computes actual photon paths through curved spacetime, producing:
- **Einstein rings** when source, lens, and observer are aligned
- **Multiple images** due to strong lensing
- **Distortion** of background objects

### Critical Impact Parameters

1. **Direct impact**: $b > 3r_s/2$ - photon reaches observer
2. **One orbit**: $b \approx 3r_s/2$ - photon orbits once before escaping  
3. **Capture**: $b < 3r_s/2$ - photon spirals into black hole

---

## N-Body Gravitational Dynamics

### Newton's Law of Universal Gravitation

For the classical gravitational interactions between celestial objects:

$$\vec{F}_{ij} = -G\frac{m_i m_j}{|\vec{r}_{ij}|^3}\vec{r}_{ij}$$

Where $\vec{r}_{ij} = \vec{r}_j - \vec{r}_i$ is the position vector from object $i$ to object $j$.

### Implementation in Physics.cpp

```cpp
void Physics::calculateGravitationalForces() {
    for (size_t i = 0; i < m_objects.size(); ++i) {
        for (size_t j = i + 1; j < m_objects.size(); ++j) {
            glm::vec3 displacement = obj2.getPosition() - obj1.getPosition();
            double distance = glm::length(displacement);
            
            if (distance > 0.0) {
                // F = G * m1 * m2 / r²
                double forceMagnitude = m_G * obj1.getMass() * obj2.getMass() / (distance * distance);
                glm::vec3 forceDirection = glm::normalize(displacement);
                glm::vec3 force = forceDirection * static_cast<float>(forceMagnitude);
                
                // Newton's third law
                obj1.applyForce(force);
                obj2.applyForce(-force);
            }
        }
    }
}
```

### Black Hole Gravitational Field

The central black hole exerts force on all objects:

$$\vec{F} = -\frac{GMm}{r^2}\hat{r}$$

With special handling near the event horizon to avoid numerical singularities.

---

## Numerical Integration Methods

### Euler Method

The simplest integration scheme:

$$\vec{v}_{n+1} = \vec{v}_n + \vec{a}_n \Delta t$$
$$\vec{r}_{n+1} = \vec{r}_n + \vec{v}_n \Delta t$$

**Pros**: Fast, simple
**Cons**: Energy drift, instability for orbital mechanics

### Leapfrog Integration

A symplectic integrator that conserves energy better:

$$\vec{v}_{n+1/2} = \vec{v}_{n-1/2} + \vec{a}_n \Delta t$$
$$\vec{r}_{n+1} = \vec{r}_n + \vec{v}_{n+1/2} \Delta t$$

### Runge-Kutta 4th Order (RK4)

Most accurate method implemented:

$$k_1 = f(t_n, y_n)$$
$$k_2 = f(t_n + \frac{\Delta t}{2}, y_n + \frac{\Delta t}{2}k_1)$$
$$k_3 = f(t_n + \frac{\Delta t}{2}, y_n + \frac{\Delta t}{2}k_2)$$
$$k_4 = f(t_n + \Delta t, y_n + \Delta t k_3)$$

$$y_{n+1} = y_n + \frac{\Delta t}{6}(k_1 + 2k_2 + 2k_3 + k_4)$$

### Implementation Choice

```cpp
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
```

---

## Relativistic Effects

### Time Dilation

Near a black hole, time runs slower due to gravitational redshift. The time dilation factor is:

$$\frac{d\tau}{dt} = \sqrt{g_{tt}} = \sqrt{1 - \frac{r_s}{r}}$$

```cpp
double BlackHole::getTimeDilationFactor(double radius) const {
    double g_tt = getMetricCoefficient(radius);
    if (g_tt <= 0.0) {
        return 0.0;  // Time stops at event horizon
    }
    return std::sqrt(g_tt);
}
```

### Gravitational Redshift

Photons climbing out of a gravitational well lose energy:

$$\frac{\nu_{\infty}}{\nu_0} = \sqrt{1 - \frac{r_s}{r}}$$

### Frame Dragging (Future Implementation)

For rotating black holes (Kerr metric), spacetime itself is dragged around:

$$\omega = \frac{2GMa}{r(r^2 + a^2\cos^2\theta)}$$

Where $a = J/(Mc)$ is the specific angular momentum.

---

## Computational Implementation

### Spacetime Curvature Visualization

The grid system visualizes spacetime curvature using the gravitational potential:

```cpp
float BlackHole::getSpacetimeCurvature(const glm::vec3& point) const {
    double r = glm::length(point - m_position);
    
    if (r <= m_schwarzschildRadius) {
        // Inside event horizon - extreme curvature
        return static_cast<float>(2.0 * std::sqrt(m_schwarzschildRadius * m_schwarzschildRadius)) - 3e10f;
    }
    
    // Gravitational potential creates curvature visualization
    double curvature = 2.0 * std::sqrt(m_schwarzschildRadius * (r - m_schwarzschildRadius));
    return static_cast<float>(curvature) - 3e10f;
}
```

### Hawking Radiation

The simulation calculates the theoretical Hawking temperature:

$$T_H = \frac{\hbar c^3}{8\pi GM k_B}$$

```cpp
double BlackHole::getHawkingTemperature() const {
    return (hbar * c * c * c) / (8.0 * M_PI * G * m_mass * k_B);
}
```

For Sagittarius A*: $T_H \approx 1.5 \times 10^{-14} \, \text{K}$ - incredibly cold!

### Bekenstein-Hawking Entropy

The black hole entropy is proportional to its surface area:

$$S_{BH} = \frac{k_B c^3 A}{4\hbar G} = \frac{k_B c^3 \cdot 4\pi r_s^2}{4\hbar G}$$

```cpp
double BlackHole::getBekensteinHawkingEntropy() const {
    double area = 4.0 * M_PI * m_schwarzschildRadius * m_schwarzschildRadius;
    return (k_B * c * c * c * area) / (4.0 * hbar * G);
}
```

---

## Physical Constants and Units

### Fundamental Constants

| Constant | Symbol | Value | Units |
|----------|--------|-------|--------|
| Gravitational constant | $G$ | $6.67430 \times 10^{-11}$ | $\text{m}^3 \text{kg}^{-1} \text{s}^{-2}$ |
| Speed of light | $c$ | $299,792,458$ | $\text{m/s}$ |
| Reduced Planck constant | $\hbar$ | $1.054571817 \times 10^{-34}$ | $\text{J⋅s}$ |
| Boltzmann constant | $k_B$ | $1.380649 \times 10^{-23}$ | $\text{J/K}$ |

### Scale Considerations

The simulation operates across enormous scales:
- **Schwarzschild radius**: $\sim 10^{10} \, \text{m}$
- **Planetary orbits**: $\sim 10^{11} - 10^{12} \, \text{m}$
- **Galactic center distance**: $\sim 10^{17} \, \text{m}$

Numerical precision is critical for stable integration across these scales.

---

## Mathematical Validation

### Energy Conservation

The simulation tracks total energy conservation:

$$E_{total} = \sum_i \frac{1}{2}m_i v_i^2 - \sum_{i<j} \frac{Gm_i m_j}{r_{ij}}$$

### Virial Theorem

For gravitationally bound systems:

$$2\langle T \rangle + \langle V \rangle = 0$$

Where $T$ is kinetic energy and $V$ is potential energy.

### Orbital Velocity Verification

For circular orbits around the black hole:

$$v_{orbital} = \sqrt{\frac{GM}{r}}$$

```cpp
double Object::getOrbitalVelocity(double centralMass) const {
    const double G = 6.67430e-11;
    double radius = static_cast<double>(glm::length(m_position));
    
    if (radius <= 0.0 || centralMass <= 0.0) return 0.0;
    
    return std::sqrt(G * centralMass / radius);
}
```

---

## Conclusion

This simulation represents a sophisticated integration of classical and relativistic physics, implementing:

1. **Schwarzschild spacetime geometry** for accurate black hole representation
2. **Geodesic ray tracing** for realistic gravitational lensing
3. **N-body dynamics** with multiple integration methods
4. **Relativistic effects** including time dilation and gravitational redshift
5. **Thermodynamic properties** via Hawking radiation and entropy calculations

The mathematical framework ensures physical accuracy while maintaining computational efficiency suitable for real-time visualization of one of nature's most extreme phenomena.

The simulation serves as both an educational tool for understanding General Relativity and a computational laboratory for exploring black hole physics - from the classical realm of planetary orbits to the exotic regime near the event horizon where spacetime itself becomes the dominant player in the cosmic dance.

---

## References

1. **Schwarzschild, K.** (1916). "Über das Gravitationsfeld eines Massenpunktes nach der Einsteinschen Theorie." *Sitzungsberichte der Königlich Preussischen Akademie der Wissenschaften*, 189-196.

2. **Hawking, S. W.** (1975). "Particle Creation by Black Holes." *Communications in Mathematical Physics*, 43(3), 199-220.

3. **Bekenstein, J. D.** (1973). "Black holes and entropy." *Physical Review D*, 7(8), 2333-2346.

4. **Einstein, A.** (1915). "Die Feldgleichungen der Gravitation." *Sitzungsberichte der Königlich Preussischen Akademie der Wissenschaften*, 844-847.

5. **Chandrasekhar, S.** (1983). *The Mathematical Theory of Black Holes*. Oxford University Press.

6. **Misner, C. W., Thorne, K. S., & Wheeler, J. A.** (1973). *Gravitation*. W. H. Freeman.

7. **Event Horizon Telescope Collaboration** (2019). "First M87 Event Horizon Telescope Results I: The Shadow of the Supermassive Black Hole." *Astrophysical Journal Letters*, 875(1), L1.

8. **Luminet, J.-P.** (1979). "Image of a spherical black hole with thin accretion disk." *Astronomy and Astrophysics*, 75, 228-235.

9. **Press, W. H., et al.** (2007). *Numerical Recipes: The Art of Scientific Computing* (3rd ed.). Cambridge University Press.

10. **Carroll, S. M.** (2004). *Spacetime and Geometry: An Introduction to General Relativity*. Addison Wesley.

---

*This document represents the complete mathematical foundation underlying the 3D Black Hole Simulation, demonstrating the elegant intersection of theoretical physics, numerical analysis, and computational graphics in bringing Einstein's vision of curved spacetime to interactive life.*