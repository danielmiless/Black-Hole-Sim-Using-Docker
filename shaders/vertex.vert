/**
 * @file vertex.vert
 * @brief Basic vertex shader for full-screen quad rendering
 * 
 * This shader transforms vertex positions for rendering the compute shader
 * output texture to the full screen. Used for displaying the ray-traced
 * black hole image.
 */

#version 330 core

// Input vertex attributes
layout (location = 0) in vec2 aPos;        // Vertex position in normalized device coordinates
layout (location = 1) in vec2 aTexCoord;   // Texture coordinates

// Output to fragment shader
out vec2 TexCoord;

void main() {
    // Pass through position (already in NDC space [-1,1])
    gl_Position = vec4(aPos, 0.0, 1.0);
    
    // Pass through texture coordinates to fragment shader
    TexCoord = aTexCoord;
}