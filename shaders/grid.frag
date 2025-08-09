/**
 * @file grid.frag
 * @brief Fragment shader for spacetime grid rendering
 * 
 * This shader applies coloring and transparency effects to the spacetime
 * grid, creating a visual representation of curved spacetime around
 * massive objects like black holes.
 */

#version 330 core

// Input from vertex shader
in float vertexAlpha;
in float distanceFromCenter;

// Output color
out vec4 FragColor;

// Uniforms for customization
uniform vec3 gridColor = vec3(0.0, 0.8, 1.0);  // Cyan-blue grid color
uniform bool useDistanceColoring = true;        // Color based on distance
uniform float pulseSpeed = 2.0;                // Animation pulse speed
uniform float time;                             // Animation time

void main() {
    vec3 finalColor = gridColor;
    float finalAlpha = vertexAlpha;
    
    // Apply distance-based coloring if enabled
    if (useDistanceColoring) {
        // Closer to black hole = more red/orange (representing intense gravity)
        // Further away = more blue (representing normal spacetime)
        
        float normalizedDistance = clamp(distanceFromCenter / 3e11, 0.0, 1.0);
        
        // Color transition: Red (close) -> Orange -> Yellow -> Cyan (far)
        if (normalizedDistance < 0.3) {
            // Very close - intense red/orange
            finalColor = mix(vec3(1.0, 0.2, 0.0), vec3(1.0, 0.6, 0.0), normalizedDistance / 0.3);
        } else if (normalizedDistance < 0.6) {
            // Medium distance - yellow to cyan transition
            float t = (normalizedDistance - 0.3) / 0.3;
            finalColor = mix(vec3(1.0, 0.6, 0.0), vec3(0.5, 0.8, 1.0), t);
        } else {
            // Far distance - blue/cyan
            finalColor = mix(vec3(0.5, 0.8, 1.0), vec3(0.0, 0.8, 1.0), (normalizedDistance - 0.6) / 0.4);
        }
    }
    
    // Add subtle pulsing animation near the black hole
    if (distanceFromCenter < 1e11) {
        float pulseIntensity = (1e11 - distanceFromCenter) / 1e11;  // Stronger pulse closer to BH
        float pulse = 0.5 + 0.5 * sin(time * pulseSpeed);
        finalAlpha *= (1.0 + pulseIntensity * pulse * 0.3);
    }
    
    // Ensure alpha doesn't exceed 1.0
    finalAlpha = clamp(finalAlpha, 0.0, 1.0);
    
    // Apply final color with computed alpha
    FragColor = vec4(finalColor, finalAlpha);
}