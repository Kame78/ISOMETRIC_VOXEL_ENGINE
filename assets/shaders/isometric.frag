#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float LightingFactor; 
in vec4 FragColorValue;

void main() {
    vec3 baseColor = FragColorValue.rgb;

    // 🔑 Directional shading based on normal direction
    float faceShade = 1.0;
    if (Normal.y > 0.5)      faceShade = 1.0;  // Top face
    else if (Normal.x > 0.5) faceShade = 0.85; // Right face
    else if (Normal.z > 0.5) faceShade = 0.70; // Front face
    else                     faceShade = 0.50; // Shadows/Back
    
    // Base block lighting calculation
    vec3 finalColor = baseColor * faceShade * (LightingFactor * 1.2);
    
    // =================================================================
    // 🔑 PROCEDURAL TACTICAL GRID BORDER OVERLAY
    // =================================================================
    float thickness = 0.04; // Adjust this value to make lines thicker or thinner
    bool isBorder = false;

    if (Normal.y > 0.5) {
        // --- TOP FACES ---
        // TexCoords run from 0.0 to 1.0 across the square tile.
        // Mark pixels that sit close to any of the 4 outer edges.
        if (TexCoords.x < thickness || TexCoords.x > (1.0 - thickness) ||
            TexCoords.y < thickness || TexCoords.y > (1.0 - thickness)) {
            isBorder = true;
        }
    } else {
        // --- SIDE WALLS (CLIFTS/DROPS) ---
        // TexCoords.x tiles continuously up to 8.0, TexCoords.y is 0.0 to 1.0.
        // Using fract() extracts the individual 0.0 -> 1.0 grid for each block column.
        float localX = fract(TexCoords.x);

    }

    // If the pixel belongs to a boundary edge, darken it to create a crisp contour line
    if (isBorder) {
        finalColor *= 0.65; // Multiplier < 1.0 darkens; make it lower for darker borders
    }
    // =================================================================

    FragColor = vec4(finalColor, 1.0);
}