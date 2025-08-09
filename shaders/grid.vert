/**
 * @file grid.vert
 * @brief Vertex shader for spacetime curvature grid visualization
 * 
 * This shader renders the warped spacetime grid that shows how massive
 * objects curve the fabric of spacetime around them. Vertices are
 * displaced based on the gravitational field strength.
 */

#version 330 core

// Input vertex attributes
layout (location = 0) in vec3 aPos;        // World space position (pre-warped)

// Uniforms
uniform mat4 viewProj;                      // Combined view-projection matrix
uniform float time;                         // Animation time
uniform float gridAlpha = 0.3;             // Grid transparency

// Output to fragment shader
out float vertexAlpha;
out float distanceFromCenter;

void main() {
    vec3 worldPos = aPos;
    
    // Calculate distance from black hole center for fading
    distanceFromCenter = length(worldPos.xz);
    
    // Apply additional warping animation (subtle)
    float animationWarp = sin(time * 0.5 + distanceFromCenter * 0.0001) * 1e8;
    worldPos.y += animationWarp;
    
    // Transform to clip space
    gl_Position = viewProj * vec4(worldPos, 1.0);
    
    // Fade grid based on distance (closer = more visible)
    float maxDistance = 5e11;  // Maximum visible distance
    float fadeStart = 1e11;    // Distance where fading begins
    
    if (distanceFromCenter > maxDistance) {
        vertexAlpha = 0.0;
    } else if (distanceFromCenter > fadeStart) {
        float fadeFactor = 1.0 - (distanceFromCenter - fadeStart) / (maxDistance - fadeStart);
        vertexAlpha = gridAlpha * fadeFactor;
    } else {
        vertexAlpha = gridAlpha;
    }
    
    // Increase visibility near the black hole
    if (distanceFromCenter < 5e10) {
        vertexAlpha *= 1.5;
    }
}