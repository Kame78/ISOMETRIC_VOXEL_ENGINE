#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float LightingFactor; 
in vec4 FragColorValue;

void main() {
    vec3 baseColor = FragColorValue.rgb;
    
    // 🔑 FIXED: "Directional" shading based on normal direction
    // This creates depth by giving every face a unique brightness level
    float faceShade = 1.0;
    
    if (Normal.y > 0.5)      faceShade = 1.0;   // Top face: Maximum Light
    else if (Normal.x > 0.5) faceShade = 0.85;  // Right face: Medium
    else if (Normal.z > 0.5) faceShade = 0.70;  // Front face: Medium-Dark
    else                     faceShade = 0.55;  // Shadows/Back: Dark
    
    // 🔑 FIXED: We mix the lighting calculation (from the vertex shader) 
    // with the faceShade (for crisp edges).
    // Raising the LightingFactor to a power (e.g., * 1.2) increases contrast.
    vec3 finalColor = baseColor * faceShade * (LightingFactor * 1.2);
    
    FragColor = vec4(finalColor, 1.0);
}