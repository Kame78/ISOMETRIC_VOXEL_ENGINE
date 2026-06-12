#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float LightingFactor; 
in vec4 FragColorValue; // .rgb = fallback color from JSON, .a = texture layer index

uniform sampler2DArray uTextureArray;

void main() {
    vec4 sampledColor = vec4(1.0);
    int layerIndex = int(FragColorValue.a + 0.5);

    // 1. Hardware Texture Lookup
    if (layerIndex >= 0) {
        sampledColor = texture(uTextureArray, vec3(TexCoords, float(layerIndex)));

        // Alpha testing discard to ensure crisp alpha transparency clip outs
        if (sampledColor.a < 0.1) {
            discard;
        }
    }

    // 2. Tinting Separation
    vec3 baseColor;
    if (layerIndex >= 0) {
        // If a valid texture layer is found, keep the raw artwork completely pristine
        baseColor = sampledColor.rgb; 
    } else {
        // Only use the flat JSON array colors if the block is completely untextured
        baseColor = FragColorValue.rgb; 
    }

    // Directional shading based on normal direction
    float faceShade = 1.0;
    if (Normal.y > 0.5)      faceShade = 1.0;  // Top face
    else if (Normal.x > 0.5) faceShade = 0.85; // Right face
    else if (Normal.z > 0.5) faceShade = 0.70; // Front face
    else                     faceShade = 0.50; // Shadows/Back
    
    // Base block lighting calculation
    vec3 finalColor = baseColor * faceShade * (LightingFactor * 1.2);
    
    // =================================================================
    // 3. PROCEDURAL TACTICAL GRID BORDER OVERLAY
    // =================================================================
    float thickness = 0.01; 
    bool isBorder = false;

    // Draw tactical grid profiles ONLY on the top faces of non-foliage blocks
    if (layerIndex != 2) {
        if (Normal.y > 0.5) {
            // --- TOP FACES ONLY ---
            if (TexCoords.x < thickness || TexCoords.x > (1.0 - thickness) ||
                TexCoords.y < thickness || TexCoords.y > (1.0 - thickness)) {
                isBorder = true;
            }
        }
        // 🔑 NOTE: Side walls are completely omitted. They will receive absolutely zero grid lines!
    }

    // If the pixel belongs to a boundary edge, darken it to create a crisp contour line
    if (isBorder) {
        finalColor *= 0.65; 
    }

    FragColor = vec4(finalColor, 1.0);
}