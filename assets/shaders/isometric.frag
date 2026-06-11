#version 330 core

out vec4 FragColor;

// Inputs from Vertex Shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float LightingFactor; 
in vec4 FragColorValue;  

void main() {

   

    // ========================================================
    // UNCONDITIONAL TEMPORARY OVERRIDE
    // Completely ignores vertex colors, json, and textures
    // ========================================================
    
    // 1. Procedural Block Coloring based on Face Direction and Height
    vec3 blockColor = vec3(0.5, 0.3, 0.1); // Default Face: Dirt Brown
    
    if (Normal.y > 0.5) {
        blockColor = vec3(0.4, 0.8, 0.2);  // Top Face: Always Grass Green
    } else if (FragPos.y < 2.0) {
        blockColor = vec3(0.6, 0.6, 0.6);  // Deep Blocks: Stone Grey
    }

    // 2. Fake Directional Lighting (Creates crisp 3D edges without wireframes)
    float faceShade = 1.0;
    if (Normal.y > 0.1) {
        faceShade = 1.0;   // Top faces are fully lit
    } else if (abs(Normal.x) > 0.1) {
        faceShade = 0.8;   // Left/Right faces are slightly shaded
    } else if (abs(Normal.z) > 0.1) {
        faceShade = 0.6;   // Front/Back faces are in shadow
    }

    // 3. Output final solid color mixed with your vertex lighting factor
    FragColor = vec4(blockColor * faceShade * LightingFactor, 1.0);
}