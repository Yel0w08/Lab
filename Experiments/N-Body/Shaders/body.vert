#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uPosition;
uniform mat4 uModel;

out vec3 vNormal;
out vec3 vFragPos;

void main() {
    vec3 worldPos = aPos + uPosition;
    gl_Position = uProjection * uView * uModel * vec4(worldPos, 1.0);
    vNormal = normalize(aPos);
    vFragPos = worldPos;
}
