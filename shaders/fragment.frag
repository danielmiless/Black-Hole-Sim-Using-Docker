/**
 * @file fragment.frag
 * @brief Fragment shader for displaying the compute shader output
 * 
 * This shader samples the texture containing the ray-traced black hole
 * image and outputs it to the screen. Can apply post-processing effects
 * like tone mapping or color correction.
 */

#version 330 core

// Input from vertex shader
in vec2 TexCoord;

// Output color
out vec4 FragColor;

// The texture containing the ray-traced image from compute shader
uniform sampler2D screenTexture;

// Optional post-processing uniforms
uniform float exposure = 1.0;      // HDR exposure adjustment
uniform float gamma = 2.2;         // Gamma correction
uniform bool enableToneMapping = false;

/**
 * @brief Simple Reinhard tone mapping
 * @param color HDR color to tone map
 * @return LDR color [0,1]
 */
vec3 reinhardToneMapping(vec3 color) {
    return color / (color + vec3(1.0));
}

/**
 * @brief Apply gamma correction
 * @param color Linear color
 * @return Gamma-corrected color
 */
vec3 gammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / gamma));
}

void main() {
    // Sample the ray-traced texture
    vec4 color = texture(screenTexture, TexCoord);
    
    // Apply HDR exposure
    vec3 hdrColor = color.rgb * exposure;
    
    // Optional tone mapping for HDR content
    if (enableToneMapping) {
        hdrColor = reinhardToneMapping(hdrColor);
    }
    
    // Apply gamma correction for proper display
    vec3 finalColor = gammaCorrection(hdrColor);
    
    // Preserve alpha channel
    FragColor = vec4(finalColor, color.a);
}