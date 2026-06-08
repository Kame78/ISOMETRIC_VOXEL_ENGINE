#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float LightingFactor;

uniform sampler2D uTextureAtlas;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        vec4 texColor = texture(uTextureAtlas, TexCoords);
        FragColor = vec4(texColor.rgb * LightingFactor, texColor.a);
    } else {
        FragColor = vec4(vec3(0.5, 0.5, 0.6) * LightingFactor, 1.0);
    }
}