#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec4 aColor;


out vec2 TexCoords;
out float LightingFactor;
out vec4 FragColorValue;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    TexCoords = aTexCoords;
    FragColorValue = aColor;


    vec3 lightDir = normalize(vec3(0.3, 0.5, 1.0));
    LightingFactor = max(dot(aNormal, lightDir), 0.4);
}