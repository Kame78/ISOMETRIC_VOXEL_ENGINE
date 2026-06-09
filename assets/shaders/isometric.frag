#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float LightingFactor;
in vec4 FragColorValue;

uniform sampler2D uTextureAtlas;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        vec4 texColor = texture(uTextureAtlas, TexCoords);
        vec3 finalRGB = texColor.rgb * FragColorValue.rgb * LightingFactor;
        FragColor = vec4(finalRGB, texColor.a * FragColorValue.a);
    } else {
        vec3 finalRGB = FragColorValue.rgb * LightingFactor;
        FragColor = vec4(finalRGB, FragColorValue.a);
    }
}